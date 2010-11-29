/*
 * winselect.c - an X11 program to get window information
 *
 * Copyright (c) 2007 G. Allen Morris III
 * All rights reserved
 *
 */

#define NAME "xtracker"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <assert.h>
#include <stdlib.h>

#include <signal.h>

#include <X11/Xlib.h>
#include <X11/X.h>
//#include <X11/extensions/scrnsaver.h>
#include <X11/Intrinsic.h>

//#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
//#endif

#include <time.h>
#include <stdio.h>

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

char *getatom(Display *dpy, Window target_win, Atom atom, long *length, Atom *type, int *size);

int current_virtual_terminal = -1;
int our_virtual_terminal = -1;
int tty0_file_descriptor;

char host_name[101];

#define MAXSTR 10000

void Fatal_Error(char *error, ...)
{
    fprintf(stderr, "%s\n", error);

    exit(1);
}

void make_daemon()
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
	// failed to fork, keep going with this process
	fprintf(stderr, "Failed to fork; unable to daemonize.\n");
	fprintf(stderr, "Continuing in the foreground.\n");
	return;
    }
    else if (pid != 0)
    {
	// fork successful
	exit(0);
    }
    else
    {
	// the child
	int x = setsid();
	chdir("/");
	umask(0);
	fprintf(stderr, "pid %d\n", x);
	close(0);
	close(1);
//	close(2);
    }
}

int window_error = 0;

static int focus_errors (Display *display, XErrorEvent *event)
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
    int	 idle;
    int	 desktop;
};

char *desktop_string = NULL;

int
get_desktops(Display *dpy, char **strings, int max)
{
    Atom atom;
    Atom type;
    int  size;
    long length;
    char *string;
    long desktop;
    int i = 0;
    int x = 0;
    int n = 0;
    int win_workspace = 0;

    {
        Atom *a;
	atom = XInternAtom(dpy, "_NET_SUPPORTED", 0);
	a = (Atom *)getatom(dpy, DefaultRootWindow(dpy),
		atom, &length, &type, &size);
	if (a) {
	    Atom xatom = XInternAtom(dpy, "_WIN_WORKSPACE_NAMES", 0);
	    Atom yatom = XInternAtom(dpy, "_NET_DESKTOP_NAMES", 0);
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
	atom = XInternAtom(dpy, "_WIN_WORKSPACE_NAMES", 1);
    } else if (win_workspace & 1) {
	atom = XInternAtom(dpy, "_NET_DESKTOP_NAMES", 1);
    } else {
        if (desktop_string) {
	    int x;
printf("free string");
	    XFree(desktop_string);
	    desktop_string = NULL;
	    for (x = 0 ; x < max; x++) {
		strings[x] = NULL;
	    }
	    return 0;
	}
    }
    if (win_workspace) {
        char *string;
	string = (char *)getatom(dpy, DefaultRootWindow(dpy),
		atom, &length, &type, &size);
	if (string) {
	    if (string != desktop_string) {
                XFree(desktop_string);
		bzero(strings, sizeof(strings) * max);
		desktop_string = string;
	    }
	    char *ptr = string;
	    while (i < length) {
		strings[n] = ptr+i;
		i += strlen(ptr+i) + 1;
		if (n++ >= max) {
		    fprintf(stderr, "too many desktops");
		    return max;
		}
	    }
	}
    } else {
	printf("No desktop names\n");
    }
    for (x = n ; x < max; x++) {
	strings[x] = NULL;
    }
    return n;
}

long
get_desktop(Display *dpy)
{
    Atom atom;
    Atom type;
    int  size;
    long length;
    long *c;
    long desktop;

    atom = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", 0);
    c = (long *)getatom(dpy, DefaultRootWindow(dpy),
	    atom, &length, &type, &size);
    if (c) {
	desktop = *c;
	XFree(c);
    }
    return desktop;
}

char *
getatom(
    Display *dpy,
    Window target_win,
    Atom atom,
    long *length, Atom *type, int *size)
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long nbytes;
    unsigned long bytes_after;
    unsigned char *prop;
    int status;
    int max_len = 256;

    status = XGetWindowProperty(dpy, target_win, atom, 0, (max_len+3)/4,
				False, AnyPropertyType, &actual_type,
				&actual_format, &nitems, &bytes_after,
				&prop);
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
         return(NULL);
    else 
        abort();

    if (!prop) {
	printf("xxxx\n");
	return(NULL);
    }

    *length = min(nitems * nbytes, max_len);
    *type = actual_type;
    *size = actual_format;
    return (char *)prop;
}

#include <X11/Xresource.h>

#include <getopt.h>

static struct option long_options[] =
{
    {"debug", 0, NULL, 'D'},
    {"desktops", 0, NULL, 't'},
    {"display", 1, NULL, 'd'},
    {"help", 0, NULL, '?'},
    {"home", 1, NULL, 'h'},
    {"window", 1, NULL, 'P'},
    {"verbose", 0, NULL, 'v'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
};

struct cmdlineopt {
    int debug;
    int desktops;
};

void usage(char *name)
{
    fprintf(stderr, _("Usage: winselect [options]\n"
" Program to get information on a window\n"
"\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"\n"
" -d --display {display}     set display to something other than $DISPLAY.\n"
" -D --debug                 stay in the foreground.\n"
" -v --verbose               display more information.\n"
"\n"
" -? --help     display this help and exit\n"
" -V --version  output version information and exit\n"
"\n"
    ));
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

void parse_command_line(int argc, char **argv, struct cmdlineopt *opt)
{
    int c, i;

    memset(opt, 0, sizeof(struct cmdlineopt));

    while ((c = getopt_long(argc, argv, "d:D?tvV", long_options, NULL)) != -1)
    {
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
	    case 't':
		opt->desktops = 1;
		break;
	    case 'h':
		if (optarg != NULL) {
		    home = optarg;
		} else {
		    usage(argv[0]);
		}
		break;
	    case '?':
		usage(argv[0]);
		exit(0);
	    case 'V':
		printf("tracker (%s) %s\n", PACKAGE, VERSION);
		exit(0);
	    case 'v':
		verbose++;
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

/*
 * Routine to let user select a window using the mouse
 */

Window Select_Window( Display *dpy, int screen )
{
    int status;
    Cursor cursor;
    XEvent event;
    Window target_win = None, root = RootWindow(dpy, screen);
    int buttons = 0;

    /* Make the target cursor */
    cursor = XCreateFontCursor(dpy, XC_crosshair);

    /* Grab the pointer using target cursor, letting it room all over */
    status = XGrabPointer(dpy, root, False,
			  ButtonPressMask | ButtonReleaseMask,
			  GrabModeSync, GrabModeAsync, root, cursor,
			  CurrentTime);
    if (status != GrabSuccess)
	Fatal_Error("Can't grab the mouse.");

    /* Let the user select a window... */
    while ((target_win == None) || (buttons != 0)) {
	/* allow one more event */
	XAllowEvents(dpy, SyncPointer, CurrentTime);
	XWindowEvent(dpy, root, ButtonPressMask | ButtonReleaseMask,
		     &event);
	switch (event.type) {
	case ButtonPress:
	    if (target_win == None) {
		target_win = event.xbutton.subwindow;	/* window selected */
		if (target_win == None)
		    target_win = root;
	    }
	    buttons++;
	    break;
	case ButtonRelease:
	    if (buttons > 0)	/* there may have been some down before we started */
		buttons--;
	    break;
	}
    }

    XUngrabPointer(dpy, CurrentTime);	/* Done with pointer */

    return (target_win);
}

void data( Display *dpy, Window new )
{
    char *name = "(unset)";
    struct window_info *win_info;
    int revert_to_return = RevertToParent;

    win_info = calloc(1, sizeof(struct window_info));

    if (new != None) {
	Window root;
	int dummyi;
	unsigned int dummy;

	Window child, parent, root2, *kids;
	int x, y, win_x, win_y;
	unsigned int mask;
	unsigned int nkids;
	if (XGetGeometry(dpy, new, &root, &dummyi, &dummyi,
			 &dummy, &dummy, &dummy, &dummy)
	    && new != root) {
	    new = XmuClientWindow(dpy, new);
	}
    }

    if (new) {
	win_info->window = new;

	{
	    int status;
	    XClassHint *class;

	    class = XAllocClassHint();
	    status = XGetClassHint(dpy, new, class);

	    if (status) {
		win_info->res_name = class->res_name;
		win_info->res_class = class->res_class;
	    }

	    XFree(class);
	}

	{
	    int status;

	    status = XFetchName(dpy, new, &name);
	    if (status) {
		win_info->name = name;
	    } else {
	        printf("No Name Data\n");
	    }

	}

	{
	    int status;
	    XTextProperty host;

	    status = XGetWMClientMachine(dpy, new, &host);
	    if (status) {
		win_info->host = (char *)host.value;
	    } else {
    //	    printf("No Host Data\n");
	    }

	}

	{
	    int status;
	    XTextProperty host;
	    Atom property = XInternAtom(dpy, "_TASKER", 0);

	    status = XGetTextProperty(dpy, new, &host, property);
	    if (status) {
		win_info->tasker = (char *)host.value;
	    } else {
    //	    printf(".");
	    }


	}

	{
	    int status;
	    XTextProperty role;
	    Atom property = XInternAtom(dpy, "WM_WINDOW_ROLE", 0);

	    status = XGetTextProperty(dpy, new, &role, property);
	    if (status) {
		win_info->res_role = (char *)role.value;
	    } else {
//	        printf(".");
	    }
	}
    }
    if (win_info->host)
	printf("host %s\n", win_info->host);
    if (win_info->res_name)
	printf("name %s\n", win_info->res_name);
    if (win_info->res_class)
	printf("class %s\n", win_info->res_class);
    if (win_info->res_role)
	printf("role %s\n", win_info->res_role);
    else
	printf("role \n");
    if (win_info->name)
	printf("title %s\n", win_info->name);
    printf("WID 0x%x\n", win_info->window);
}

int
main(int argc, char *argv[])
{
    int event_base, error_base;
    XtAppContext app;
    struct cmdlineopt options;
    int tty_file_descriptor;
    struct vt_stat terminal_status;
    char *deskspaces[20];
    int deskspace_count;
    Window target_win = None;
    char *name = "(unset)";
    Display *my_display;

    home = getenv("HOME");

    {
        int x;
	for (x = 0; x < 20; x++) {
	    deskspaces[x] = NULL;
	}
    }

    parse_command_line(argc, argv, &options);

    gethostname(host_name, 100);
    host_name[100] = '\0';

    my_display = XOpenDisplay(display_name);

    if (!my_display) {
	printf ("I couldn't open the display (%s). Is X runing?\n", display_name);
        exit(0);
    }
    {
	int x;
	int desktops;

	desktops = get_desktops(my_display, deskspaces, 20);
	printf("desktops %d\n", desktops);

	for (x=0; x < desktops; x++) {
	    printf("D%d %s\n", x, deskspaces[x]);
	}
    }
    
    if (!options.desktops) {
	target_win = Select_Window( my_display, 0 );

	if (target_win) {
	    data(my_display, target_win);
	}
    }
}

/* eof */
