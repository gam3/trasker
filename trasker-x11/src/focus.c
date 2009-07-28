/*
 * focus.c - an X11 program to track focus
 *
 * Copyright (c) 2007 G. Allen Morris III
 * All rights reserved
 *
 */

#define NAME "xtracker"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/kd.h>	// This is needed for the VT code

#include <assert.h>
#include <stdlib.h>

#include <signal.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/Intrinsic.h>

#include <time.h>
#include <stdio.h>

#include <fcntl.h>

#include <errno.h>

#include <locale.h>

#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <X11/Xmu/WinUtil.h>

#include <regex.h>

#include <sys/ioctl.h>
#include <linux/vt.h>

#include <netinet/in.h>

#define _(msgid) (msgid)

#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

#include <X11/Xresource.h>

#include <getopt.h>

char *getatom(Window target_win, Atom atom, long *length, Atom * type,
	      int *size);

int current_virtual_terminal = -1;
int our_virtual_terminal = -1;
int tty0_file_descriptor;

char host_name[101];

Display *my_display;

void Fatal_Error(char *error, ...)
{
    fprintf(stderr, "%s\n", error);

    exit(1);
}

void make_daemon()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
	// failed to fork, keep going with this process
	fprintf(stderr, "Failed to fork; unable to daemonize.\n");
	fprintf(stderr, "Continuing in the foreground.\n");
    } else if (pid != 0) {
	// fork successful
	exit(0);
    } else {
	// the child
	int x = setsid();
	chdir("/");
	umask(0);
	close(0);
	close(1);
        close(2);
    }
    return;
}

int window_error = 0;

static int focus_errors(Display * display, XErrorEvent * event)
{
    char buffer_return[1024];
    int length = 1024;
    /* ignore XFetchName problems */
    fprintf(stderr, "X error %d\n", event->error_code);
    XGetErrorText(display, event->error_code, buffer_return, length);
    fprintf(stderr, "X error %s\n", buffer_return);

    window_error = 1;

    return 1;
}

struct window_info {
    Window window;
    time_t start_time;
    time_t end_time;
    char *name;
    char *host;
    char *tasker;
    char *res_name;
    char *res_class;
    char *res_role;
    int idle;
    int desktop;
};


char *desktop_string = NULL;

int get_desktops(char **strings, int max)
{
    Atom atom;
    Atom type;
    int size;
    long length;
    char *string;
    long desktop;
    int i = 0;
    int x = 0;
    int n = 0;
    int win_workspace = 0;

    {
	Atom *a;
	atom = XInternAtom(my_display, "_NET_SUPPORTED", 0);
	a = (Atom *) getatom(DefaultRootWindow(my_display),
			     atom, &length, &type, &size);
	if (a) {
	    Atom xatom =
		XInternAtom(my_display, "_WIN_WORKSPACE_NAMES", 0);
	    Atom yatom = XInternAtom(my_display, "_NET_DESKTOP_NAMES", 0);
	    for (i = 0; i < size; i++, a++) {
		if (yatom == *a) {
		    win_workspace |= 1;
		}
		if (xatom == *a) {
		    win_workspace |= 2;
		}
	    }
	} else {
	}
	i = 0;
    }

    if (win_workspace & 2) {
	atom = XInternAtom(my_display, "_WIN_WORKSPACE_NAMES", 1);
    } else if (win_workspace & 1) {
	atom = XInternAtom(my_display, "_NET_DESKTOP_NAMES", 1);
    } else {
	if (desktop_string) {
	    int x;
	    printf("free string");
	    XFree(desktop_string);
	    desktop_string = NULL;
	    for (x = 0; x < max; x++) {
		strings[x] = NULL;
	    }
	    return 0;
	}
    }
    if (win_workspace) {
	char *string;
	string = (char *) getatom(DefaultRootWindow(my_display),
				  atom, &length, &type, &size);
	if (string) {
	    if (string != desktop_string) {
		XFree(desktop_string);
		bzero(strings, sizeof(strings) * max);
		desktop_string = string;
	    }
	    char *ptr = string;
	    while (i < length) {
		strings[n] = ptr + i;
		i += strlen(ptr + i) + 1;
		if (n++ >= max) {
		    fprintf(stderr, "too many desktops");
		    return max;
		}
	    }
	}
    } else {
	printf("No desktop names\n");
    }
    for (x = n; x < max; x++) {
	strings[x] = NULL;
    }
    return n;
}

long get_desktop()
{
    Atom atom;
    Atom type;
    int size;
    long length;
    long *c;
    long desktop;

    atom = XInternAtom(my_display, "_NET_CURRENT_DESKTOP", 0);
    c = (long *) getatom(DefaultRootWindow(my_display),
			 atom, &length, &type, &size);
    if (c) {
	desktop = *c;
	XFree(c);
    }
    return desktop;
}

int idle = 0;

int
test_focus(struct window_info *win_info, XScreenSaverInfo * mit_info,
	   long idletime)
{
    Window new;
    char *name = "(unset)";
    int revert_to_return = RevertToParent;
    int ret;
    struct vt_stat terminal_status;
    int (*old_handler) (Display *, XErrorEvent *);

    int flag = 0;

    {
	Atom atom = XInternAtom(my_display, "_NET_CURRENT_DESKTOP", 0);
	Atom type;
	int size;
	long length;
	long *c;
	c = (long *) getatom(DefaultRootWindow(my_display), atom, &length,
			     &type, &size);
	if (c) {
	    win_info->desktop = *c;
	    XFree(c);
	} else {
	    printf("no desktop\n");
	}
    }

/* There are a couple of pseudo windows that do not allocate memory */
    if (win_info->window > (Window) 2) {
	if (win_info->name) {
	    XFree(win_info->name);
	    win_info->name = NULL;
	}
	if (win_info->res_name) {
	    XFree(win_info->res_name);
	    win_info->res_name = NULL;
	}
	if (win_info->res_class) {
	    XFree(win_info->res_class);
	    win_info->res_class = NULL;
	}
	if (win_info->res_role) {
	    XFree(win_info->res_role);
	    win_info->res_role = NULL;
	}
	if (win_info->host) {
	    XFree(win_info->host);
	}
	if (win_info->tasker) {
	    XFree(win_info->tasker);
	    win_info->tasker = NULL;
	}
    }
    win_info->name = NULL;
    win_info->res_name = NULL;
    win_info->res_class = NULL;
    win_info->host = NULL;

    win_info->start_time = time(NULL);

    XScreenSaverQueryInfo(my_display, DefaultRootWindow(my_display),
			  mit_info);

    if (mit_info->idle > idletime) {
	if (idle == 0) {
	    fprintf(stderr, "idle\n");
	    idle = 1;
	}

	win_info->idle = 1;
    } else {
	if (idle == 1) {
	    fprintf(stderr, "active\n");
	    idle = 0;
	}
	win_info->idle = 0;
    }
    if (ioctl(tty0_file_descriptor, VT_GETSTATE, &terminal_status) == -1) {
	current_virtual_terminal = -3;
    } else if (terminal_status.v_active != our_virtual_terminal) {
	static char buf[10];
	win_info->window = 0;
	win_info->host = host_name;
	win_info->res_name = "switch";
	sprintf(buf, "%d", terminal_status.v_active);
	win_info->res_class = buf;
	win_info->idle = 1;
    }
    old_handler = XSetErrorHandler(focus_errors);

    XGetInputFocus(my_display, &new, &revert_to_return);

    if (new == None) {
	win_info->window = None;
	win_info->host = host_name;
    } else if (new == PointerRoot) {
	Window root, child, parent, root2, *kids;
	int x, y, win_x, win_y;
	unsigned int mask;
	unsigned int nkids;

	XQueryPointer(my_display, DefaultRootWindow(my_display), &root,
		      &child, &x, &y, &win_x, &win_y, &mask);
	new = child;

	flag = 1;
    }

    if (new != None) {
	Window root;
	int dummyi;
	unsigned int dummy;

	Window child, parent, root2, *kids;
	int x, y, win_x, win_y;
	unsigned int mask;
	unsigned int nkids;

	if (XQueryTree(my_display, new, &root2, &parent, &kids, &nkids))
	    if (parent != None)
		new = parent;

	if (XGetGeometry(my_display, new, &root, &dummyi, &dummyi,
			 &dummy, &dummy, &dummy, &dummy)
	    && new != root) {
	    new = XmuClientWindow(my_display, new);
	}
    }
    if (new) {
	win_info->window = new;

	{
	    int status;
	    XClassHint *class;

	    class = XAllocClassHint();
	    status = XGetClassHint(my_display, new, class);

	    if (status) {
		win_info->res_name = class->res_name;
		win_info->res_class = class->res_class;
	    }

	    XFree(class);
	}

	{
	    int status;

	    status = XFetchName(my_display, new, &name);
	    if (status) {
		win_info->name = name;
	    } else {
		//      printf("No Name Data\n");
	    }

	}

	{
	    int status;
	    XTextProperty host;

	    status = XGetWMClientMachine(my_display, new, &host);
	    if (status) {
		win_info->host = (char *) host.value;
	    } else {
		//      printf("No Host Data\n");
	    }

	}

	{
	    int status;
	    XTextProperty host;
	    Atom property = XInternAtom(my_display, "_TASKER", 0);

	    status = XGetTextProperty(my_display, new, &host, property);
	    if (status) {
		win_info->tasker = (char *) host.value;
	    } else {
		//      printf(".");
	    }


	}

	{
	    int status;
	    XTextProperty host;
	    Atom property = XInternAtom(my_display, "WM_WINDOW_ROLE", 0);

	    status = XGetTextProperty(my_display, new, &host, property);
	    if (status) {
		win_info->res_role = (char *) host.value;
	    } else {
		//      printf(".");
	    }


	}
    }

    (void) XSetErrorHandler(old_handler);
    return ret;
}

char *getatom(Window target_win,
	      Atom atom, long *length, Atom * type, int *size)
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long nbytes;
    unsigned long bytes_after;
    unsigned char *prop;
    int status;
    int max_len = 256;
    Display *dpy = my_display;

    status =
	XGetWindowProperty(my_display, target_win, atom, 0,
			   (max_len + 3) / 4, False, AnyPropertyType,
			   &actual_type, &actual_format, &nitems,
			   &bytes_after, &prop);
    if (status == BadWindow) {
	Fatal_Error("window id # 0x%lx does not exists!", target_win);
    }
    if (status != Success) {
	Fatal_Error("XGetWindowProperty failed!");
    }

    if (actual_format == 32)
	nbytes = sizeof(long);
    else if (actual_format == 16)
	nbytes = sizeof(short);
    else if (actual_format == 8)
	nbytes = 1;
    else if (actual_format == 0)
	return (NULL);
    else
	abort();

    if (!prop) {
	printf("xxxx\n");
	return (NULL);
    }

    *length = min(nitems * nbytes, max_len);
    *type = actual_type;
    *size = actual_format;
    return (char *) prop;
}

static struct option long_options[] = {
    {"debug", 0, NULL, 'D'},
    {"display", 1, NULL, 'd'},
    {"help", 0, NULL, '?'},
    {"home", 1, NULL, 'h'},
    {"idle", 1, NULL, 'i'},
    {"log", 1, NULL, 'l'},
    {"pid", 1, NULL, 'P'},
    {"port", 1, NULL, 'p'},
    {"server", 1, NULL, 's'},
    {"user", 1, NULL, 'u'},
    {"login", 1, NULL, 'L'},
    {"verbose", 0, NULL, 'v'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
};

struct cmdlineopt {
    int debug;
    long idle;
    int kill;
};

void usage(char *name)
{
    fprintf(stderr, _("Usage: focus [options]\n"
		      " Focus tracking daemon\n"
		      "\n"
		      "Mandatory arguments to long options are mandatory for short options too.\n"
		      "\n"
		      " -d --display {display}     set display to something other than $DISPLAY.\n"
		      " -D --debug                 stay in the foreground.\n"
		      " -k --kill                  kill any running clients.\n"
		      " -s --server {ip address}   set servers ip address [127.0.0.1].\n"
		      " -p --port {port #}         set servers port number [8080].\n"
		      " -u --user {user}           set user [$USER]\n"
		      " -v --verbose               display more information.\n"
		      " -P --pid {file}            use other than default pid file.\n"
		      " -l --log {file}            use other than default log file.\n"
		      " -L --login {u/p}           the authorization string.\n"
		      " -h --home {path}           use this instead of $HOME for log and pid files.\n"
		      "\n"
		      " -? --help     display this help and exit\n"
		      " -V --version  output version information and exit\n"
		      "\n"));
}

char *home;
char *pid_file = NULL;
char *log_file = NULL;
char *display_name = NULL;
int port = 8000;
char *server = "127.0.0.1";
int pid;
char *user;
int verbose = 0;

char *config_file = "/home/gam3/.config/Tasker/user.conf";

void get_password()
{
    FILE *config;
    char buffer[1024];
    char *ptr;

    if (!(config = fopen(config_file, "r"))) {
	perror("Could not open logfile");
	exit(-3);
    }
    while (ptr = fgets(buffer, 1024, config)) {
printf("%s", ptr);
    }
}

void parse_command_line(int argc, char **argv, struct cmdlineopt *opt)
{
    int c, i;

    memset(opt, 0, sizeof(struct cmdlineopt));

    while ((c =
	    getopt_long(argc, argv, "d:D?i:kP:p:s:u:v", long_options,
			NULL)) != -1) {
	switch (c) {
	case 'D':
	    opt->debug = 1;
	    break;
	case 'd':
	    if (optarg != NULL) {
		display_name = optarg;
	    } else {
		usage(argv[0]);
	    }
	    break;
	case 'h':
	    if (optarg != NULL) {
		home = optarg;
	    } else {
		usage(argv[0]);
	    }
	    break;
	case 'i':
	    opt->idle = atol(optarg);
	    break;
	case '?':
	    usage(argv[0]);
	    exit(0);
	case 'l':
	    if (optarg != NULL) {
		log_file = optarg;
	    } else {
		usage(argv[0]);
	    }
	    break;
	case 'L':
	    if (optarg != NULL) {
		char *ptr;
//		user = optarg;
                for (ptr = optarg; *ptr; ptr++) {
		    printf("%p %c\n", ptr, *ptr);
		    *ptr = '*';
		}
	    } else {
		usage(argv[0]);
		exit(0);
	    }
	    break;
	case 's':
	    if (optarg != NULL) {
		server = optarg;
	    } else {
		usage(argv[0]);
		exit(0);
	    }
	    break;
	case 'u':
	    if (optarg != NULL) {
		user = optarg;
	    } else {
		usage(argv[0]);
		exit(0);
	    }
	    break;
	case 'p':
	    if (optarg != NULL) {
		port = atoi(optarg);
	    } else {
		usage(argv[0]);
	    }
	    break;
	case 'P':
	    if (optarg != NULL) {
		pid_file = optarg;
	    } else {
		usage(argv[0]);
	    }
	    break;
	case 'V':
	    printf("tracker (%s) %s\n", PACKAGE, VERSION);
	    exit(0);
	case 'v':
	    verbose++;
	    break;
	case 'k':
	    opt->kill = 1;
	    break;
	default:
	    printf("Try `%s --help' for more information.\n", argv[0]);
	    exit(1);
	}
    }
}

int stay = 1;

void sighandle(int x)
{
    fprintf(stderr, "interupt %d\n", x);
    stay = 0;
    return;
}

void sigpipe(int x)
{
    fprintf(stderr, "sigpipe\n", x);
    return;
}

void loop(int, long, char **, int);

#define SLOTS  20
#define DELAY  500000L

int wdiff(struct window_info *a, struct window_info *b)
{
    int ret = (a->window != b->window);
    ret += (a->desktop != b->desktop);
    if (!ret && a->name && b->name) {
	ret += strcmp(a->name, b->name) != 0;
    }
    if (a->idle != b->idle) {
	ret += 1;
    }

    return ret;
}

void log_window(FILE * log, struct window_info *current_window_info)
{
    if (log) {
	if (current_window_info->start_time) {
	    fprintf(log, "%lu %lu %lu %s.%s.%s.\"%s\"\n",
		    current_window_info->start_time,
		    time(NULL),
		    current_window_info->window,
		    current_window_info->host,
		    current_window_info->res_name,
		    current_window_info->res_class,
		    current_window_info->name);
	}
    }
}

void
rdf_window(FILE * frdf, struct window_info *current_window_info,
	   char **desktops)
{
    char *desktop = desktops[current_window_info->desktop];

    if (frdf) {
	if (current_window_info->start_time) {
	    if (verbose)
		fprintf(stderr,
			"tracker\t%s\t%s\t%d\t%s\t%lu\t%s\t%s\t%s\t%s\t%s\n",
			user, host_name, current_window_info->idle,
			desktops[current_window_info->desktop],
			current_window_info->window,
			current_window_info->host,
			current_window_info->res_name,
			current_window_info->res_class,
			current_window_info->res_role,
			current_window_info->name);
	    fprintf(frdf,
		    "tracker\t%s\t%s\t%d\t%s\t%lu\t%s\t%s\t%s\t%s\t%s\n",
		    user, host_name, current_window_info->idle,
		    desktops[current_window_info->desktop],
		    current_window_info->window, current_window_info->host,
		    current_window_info->res_name,
		    current_window_info->res_class,
		    current_window_info->res_role,
		    current_window_info->name);
	} else {
	    fprintf(stderr, "no starttime\n");
	}
    }
}

void rdf_xwindow(FILE * frdf, struct window_info *current_window_info)
{
    if (frdf) {
	if (current_window_info->start_time) {
	    if (verbose > 1)
		fprintf(stderr, "xtracker\t%s\t%s\t%lu\t%s\t%s\t%s\t%s\n",
			user,
			host_name,
			current_window_info->window,
			current_window_info->host,
			current_window_info->res_name,
			current_window_info->res_class,
			current_window_info->name);
	    fprintf(frdf, "xtracker\t%s\t%s\t%lu\t%s\t%s\t%s\t%s\n",
		    user,
		    host_name,
		    current_window_info->window,
		    current_window_info->host,
		    current_window_info->res_name,
		    current_window_info->res_class,
		    current_window_info->name);
	} else {
	    fprintf(stderr, "x no starttime\n");
	}
    }
}

void loop(int rfd, long idletime, char **desktops, int desktopcnt)
{
    struct window_info win_info[SLOTS];
    struct window_info current_window_info;
    struct window_info temp_window_info;
    FILE *log = NULL;
    int idle_flag = 0;

    XScreenSaverInfo *mit_info;

    int x;

    mit_info = XScreenSaverAllocInfo();

    long desktop[SLOTS];

    long current_desktop;
    time_t start_time = time(NULL);

    FILE *frfd;

    if (rfd < 0) {
	rfd = get_remotefd(server, port);
    }

    if (rfd >= 0) {
	frfd = fdopen(rfd, "w+");
    } else {
	rfd = -1;
	frfd = NULL;
    }

    for (x = 0; x < SLOTS; x++) {
	memset(&win_info[x], 0, sizeof(struct window_info));
	desktop[x] = 0L;
    }

    if (frfd) {
	fprintf(frfd, "host\t%s:%d\n", host_name, display_name);
    }

    memset(&current_window_info, 0, sizeof(current_window_info));
    current_window_info.start_time = 0;
    current_window_info.host = host_name;
    current_window_info.window = 1;

    memset(&temp_window_info, 0, sizeof(struct window_info));

    x = 0;

    test_focus(&win_info[x], mit_info, idletime);
    memmove(&current_window_info, &win_info[x],
	    sizeof(struct window_info));
    get_desktops(desktops, 20);
    rdf_window(frfd, &current_window_info, desktops);

    while (stay) {
	struct vt_stat terminal_status;
	int i, j;
	int miss = 0;
	int desk_miss = 0;
	int force = 0;

	int count;

	if (frfd) {
	    char *ptr;
	    char buffer[1024];
	    count = -1;
	    ptr = fgets(buffer, 1024, frfd);

	    if (ptr == NULL) {
		if (errno == 0) {
		    /* we do not get here until there is a write */
		    frfd = NULL;
		    rfd = -1;
		    fprintf(stderr, "Disconected\n");
		} else if (errno != EAGAIN) {
		    perror("Error reading socket");
		} /* else {
		     We got an EAGAIN
		} */
	    } else {
		buffer[strlen(buffer) - 1] = '\0';
		printf("buffer: %s\n", buffer);
	    }
	}

	current_desktop = get_desktop();

	for (i = 0; i < SLOTS; i++) {
	    if (i == x)
		continue;
	    if (wdiff(&win_info[i], &win_info[x])) {
		miss = 1;
	    }
#if 0
	    if (desktop[i] != desktop[x]) {
		desk_miss = 1;
	    }
#else
	    desk_miss = 1;
#endif
	}
	if ((!miss && wdiff(&win_info[x], &current_window_info)) ||
	    (!desk_miss && desktop[x] != current_desktop)) {
	    if (!(log = fopen(log_file, "a"))) {
		perror("Could not open logfile");
	    }
	    if (rfd < 0) {
		rfd = get_remotefd(server, port);
		if (rfd >= 0) {
		    frfd = fdopen(rfd, "w+");
		    fprintf(stderr, "Running in Connected mode\n");
		}
	    }
	}
	XScreenSaverQueryInfo(my_display, DefaultRootWindow(my_display),
			      mit_info);

	if (mit_info->idle > idletime) {
	    if (idle_flag != 1) {
		idle_flag = 1;
		miss = 0;
		force = 1;
	    }
	} else {
	    if (idle_flag == 1) {
		idle_flag = 0;
		miss = 0;
		force = 1;
	    }
	}

	if ((!miss && wdiff(&win_info[x], &current_window_info)) || force) {
	    get_desktops(desktops, 20);
	    log_window(log, &current_window_info);
	    rdf_xwindow(frfd, &current_window_info);
	    memmove(&current_window_info, &win_info[x],
		    sizeof(struct window_info));
	    memset(&win_info[x], 0, sizeof(struct window_info));
	    rdf_window(frfd, &current_window_info, desktops);
	    force = 0;
	}
// if all desktops same and not same as old desktop
	if (!desk_miss && desktop[x] != current_desktop) {
	    if (log && start_time) {
		fprintf(log, "%lu %lu Desktop %d \"%s\"\n",
			start_time,
			time(NULL),
			current_desktop, desktops[current_desktop]
		    );
	    }
	    assert(0);
	    if (frfd) {
		fprintf(frfd, "xtracker\t%lu\t%lu\tDesktop\t%d\t%s\n",
			start_time,
			time(NULL),
			current_desktop, desktops[current_desktop]
		    );
	    }
	    current_desktop = get_desktop();
	    start_time = time(NULL);
	    if (frfd) {
		fprintf(frfd, "desktop\t%s\t%s\t%lu\t%d\t%s\n",
			user,
			host_name,
			start_time,
			current_desktop, desktops[current_desktop]
		    );
	    }
	}
	if (frfd) {
	    int ref = fflush(frfd);
	    if (ref < 0) {
		fprintf(stderr, "Disconected\n");
		if (log) {
		    fprintf(log, "%ld %ld Disconected\n", time(NULL),
			    time(NULL));
		}
		rfd = -1;
		frfd = NULL;
	    }
	    if (rfd < 0) {
		rfd = get_remotefd(server, port);
		if (rfd >= 0) {
		    frfd = fdopen(rfd, "w+");
		    fprintf(stderr, "Running in Connected mode\n");
		}
	    }
	}
	if (log) {
	    fflush(log);
	    fclose(log);
	    log = NULL;
	}

	x = (x + 1) % SLOTS;

	usleep(DELAY);

	desktop[x] = get_desktop();
	test_focus(&win_info[x], mit_info, idletime);
    }

    if (!(log = fopen(log_file, "a"))) {
	perror("Could not open logfile");
    }
    if (log) {
	fprintf(log, "%lu %lu Desktop %d \"%s\"\n",
		start_time,
		time(NULL), current_desktop, desktops[current_desktop]
	    );
	log_window(log, &current_window_info);
    }
    if (frfd) {
	fprintf(frfd, "%lu %lu Desktop %d \"%s\"\n",
		start_time,
		time(NULL), current_desktop, desktops[current_desktop]
	    );
	rdf_window(frfd, &current_window_info, desktops);
    }
    if (frfd) {
	fflush(frfd);
	fclose(frfd);
    }
    if (log) {
	fflush(log);
	fclose(log);
	log = NULL;
    }
}

int get_remotefd(char *server_addr_s, int port)
{
    struct sockaddr_in remoteaddr;
    int remotefd = -1;
    int in_val;

    if (0 == inet_aton(server_addr_s, &in_val)) {
	fprintf(stderr, "Bad ip address %s\n", server_addr_s);
	perror("Bad ip address\n");
	exit(1);
    }
    if ((remotefd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
	memset(&remoteaddr, 0, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = in_val;
	remoteaddr.sin_port = htons(port);
	if (connect
	    (remotefd, (struct sockaddr *) &remoteaddr,
	     sizeof(remoteaddr)) < 0) {
	    remotefd = -1;
	}
    } else {
	remotefd = -1;
    }
    if (remotefd > 0) {
	int flags;
	flags = fcntl(remotefd, F_GETFL);
	if (flags == -1) {
	    perror("getting flags");
	    exit(1);
	}
	flags = fcntl(remotefd, F_SETFL, flags | O_NONBLOCK);
	if (flags == -1) {
	    perror("setting flags");
	    exit(1);
	}
    }

    return remotefd;
}

int is_a_console(int fd)
{
    char arg;

    arg = 0;
    if (ioctl(fd, KDGKBTYPE, &arg) == 0) {
	printf("arg = %d\n", arg);
	return ((arg == KB_101) || (arg == KB_84));
    }
    return 0;
}

static int open_a_console(char *fnam)
{
    int fd;

    /* try read-only */
    fd = open(fnam, O_RDWR);

    /* if failed, try read-only */
    if (fd < 0 && errno == EACCES)
	fd = open(fnam, O_RDONLY);

    /* if failed, try write-only */
    if (fd < 0 && errno == EACCES)
	fd = open(fnam, O_WRONLY);

    /* if failed, fail */
    if (fd < 0)
	return -1;

    /* if not a console, fail */
    if (!is_a_console(fd)) {
	close(fd);
	return -1;
    }

    /* success */
    return fd;
}

int get_console_fd(char *tty_name)
{
    int fd;

    if (tty_name) {
	if (-1 == (fd = open_a_console(tty_name)))
	    return -1;
	else
	    return fd;
    }

    fd = open_a_console("/dev/tty");
    if (fd >= 0)
	return fd;

    fd = open_a_console("/dev/tty0");
    if (fd >= 0)
	return fd;

    fd = open_a_console("/dev/console");
    if (fd >= 0)
	return fd;

    for (fd = 0; fd < 3; fd++)
	if (is_a_console(fd))
	    return fd;

    return -1;			/* total failure */
}

int main(int argc, char *argv[])
{
    int ret;
    int event_base, error_base;
    XtAppContext app;
    struct cmdlineopt options;
    struct vt_stat terminal_status;
    int rfd;
    int pid_fd;
    char *deskspaces[20];
    int deskspace_count;

    home = getenv("HOME");

    {
	int x;
	for (x = 0; x < 20; x++) {
	    deskspaces[x] = NULL;
	}
    }

    parse_command_line(argc, argv, &options);
    get_password();

    long idletime = 3 * 60 * 1000L;

    typedef void (*sighandler_t) (int);
    sighandler_t old_sighander;


    user = getenv("USER");

    gethostname(host_name, 100);
    host_name[100] = '\0';

    my_display = XOpenDisplay(display_name);

    if (!my_display) {
	printf("I couldn't open the display (%s). Is X runing?\n",
	       display_name);
	exit(0);
    }

    {
	char *buffer;
	char *x = strdup(XDisplayString(my_display));
	char *t = strchr(x, ':');
	char *host = x;
	char *port;

	if (t) {
	    *t++ = '\0';
	}
	port = t;
	t = strchr(x, '.');
	if (t) {
	    *t = '\0';
	}
	if (!*x) {
	    host = host_name;
	}
	buffer = calloc(1024, 1);
	if (!log_file) {
	    sprintf(buffer, "%s/.tasker.%s.%d.%s", home, host, atoi(port),
		    "log");
	    log_file = strdup(buffer);
	}
	if (!pid_file) {
	    sprintf(buffer, "%s/.tasker.%s.%d.%s", home, host, atoi(port),
		    "pid");
	    pid_file = strdup(buffer);
	}
	free(buffer);
	free(x);
    }

    old_sighander = signal(SIGQUIT, &sighandle);
    old_sighander = signal(SIGTERM, &sighandle);
    old_sighander = signal(SIGINT, &sighandle);
    old_sighander = signal(SIGPIPE, &sigpipe);

    if (options.kill) {
	char vbuf[33];
	int fd = open(pid_file, O_EXCL | O_RDONLY);
	if (fd >= 0) {
	    struct stat buf;
	    int ret = -1;
	    int cnt = read(fd, &vbuf, 32);
	    int pid = atoi(vbuf);
	    if (pid) {
		fprintf(stderr, "Killing %d\n", pid);
		ret = kill(pid, SIGQUIT);
		usleep(500000);	// .5 seconds
		fstat(fd, &buf);
	    }
	    if (ret == 0) {
		fprintf(stderr, "%d killed\n", pid);
	    } else {
		if (errno == ESRCH) {
		    fprintf(stderr, "%d was not running.\n", pid);
		} else {
		    fprintf(stderr, "Unknown error killing %d.\n", pid);
		}
		ret = unlink(pid_file);
		if (ret < 0) {
		    perror("Could not removed pid file.\n");
		} else {
		    fprintf(stderr, "pid file removed.\n");
		}
	    }
	    close(fd);
	} else {
	    if (errno == ENOENT) {
		fprintf(stderr, "No tracker seems to be running.\n");
	    } else {
		perror("pid file error");
	    }
	}
	exit(0);
    }

    rfd = get_remotefd(server, port);

    if (rfd < 0) {
	fprintf(stderr, "Running in Disconnected mode\n");
    }

    if (options.idle) {
	int x = options.idle;
	idletime = x * 1000;
    }

    if (verbose)
	printf("Idletime = %ld\n", idletime);

    app = XtCreateApplicationContext();

    setlocale(LC_CTYPE, "");

    {
	char buffer[81];
	char host[128];

	int display;
	int screen;
	strncpy(buffer, XDisplayString(my_display), 80);
	buffer[80] = '\0';
	if (buffer[0] == ':') {
	    char *x = strcpy(buffer, host_name);
	    strncat(buffer, XDisplayString(my_display),
		    80 - strlen(buffer));
	}
	buffer[80] = '\0';
	{
	    char *x = strchr(buffer, ':');
	    *x = ' ';
	}
	int x = sscanf(buffer, "%s %d.%d", &host, &display, &screen);
    }

    if (verbose)
	printf("Screen count: %d\n", ScreenCount(my_display));

    {
	Atom atom = XInternAtom(my_display, "XFree86_VT", 0);
	Atom type;
	int size;
	long length;
	char *c;

	c = getatom(DefaultRootWindow(my_display),
		    atom, &length, &type, &size);
	if (c) {
	    if (*(long *) c != our_virtual_terminal)
		our_virtual_terminal = *(long *) c;
	    XFree(c);
	}
    }
    if (verbose)
	printf("Virtual screen: %ld\n", our_virtual_terminal);

    tty0_file_descriptor = get_console_fd(NULL);
    if (tty0_file_descriptor == -1) {
	current_virtual_terminal = our_virtual_terminal;
    } else {
	if (ioctl(tty0_file_descriptor, VT_GETSTATE, &terminal_status) ==
	    -1) {
	    if (current_virtual_terminal == -1)
		printf
		    ("An error occurred while executing the ioctl. No information was supplied.\n");
	    current_virtual_terminal = -3;
	}
	if (current_virtual_terminal == -1) {
	    our_virtual_terminal = terminal_status.v_active;
	    current_virtual_terminal = terminal_status.v_active;
	}
    }

    if (pid_file != NULL) {
	/*
	 * unlink the pid_file, if it exists, prior to open.  Without
	 * doing this the open will fail if the user specified pid_file
	 * already exists.
	 */
	pid_fd = open(pid_file, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (pid_fd == -1) {
	    perror(pid_file);
	    if (errno == 29) {
		printf("Try: " "focus" " -k to kill the process and remove the file.\n");
	    }
	    exit(1);
	}
    }
    if (!options.debug) {
	make_daemon();
    }
    if (pid_fd) {
	FILE *PID;
	if ((PID = fdopen(pid_fd, "w")) == NULL) {
	    perror("Could not fdopen pid file");
	    exit(1);
	} else {
	    fprintf(PID, "%d\n", (int) getpid());
	    fclose(PID);
	}
	close(pid_fd);
    }

    loop(rfd, idletime, deskspaces, deskspace_count);

    if (pid_file) {
	int x;
	x = unlink(pid_file);
    }
}

/* eof */
