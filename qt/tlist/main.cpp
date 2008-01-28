
#include <qapplication.h>
#include <qstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tlist.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "global.h"
#include "ttcp.h"

void parse_command_line(int argc, char **argv, struct cmdlineopt *opt);

int main(int argc, char *argv[])
{
    struct sockaddr_in remoteaddr;
    struct cmdlineopt options;
    options.user = getenv("USER");
    options.port = 8000;
    options.server = "localhost";
    options.verbose = 0;
    options.iconify = 0;
    options.display = NULL;
    QString host;

    QApplication a(argc, argv);

    parse_command_line(a.argc(), a.argv(), &options);
    host = options.server;

    TTCP *server = new TTCP(host, options.port, false);

    TTListView *listview = new TTListView(options.user, server, options.verbose);

    listview->setName("tasklist");

    if (options.iconify)
	listview->iconify();

    QObject::connect(listview,
		     SIGNAL(rightButtonPressed
			    (QListViewItem *, const QPoint &, int)),
		     listview,
		     SLOT(slotRMB(QListViewItem *, const QPoint &, int)));

    listview->addColumn(QString("Name"), 300);
    listview->addColumn(QString("Time"), 100);
    listview->addColumn(QString("Id"), 100);
    listview->setColumnAlignment( 1, Qt::AlignRight );
    listview->setColumnAlignment( 2, Qt::AlignRight );
    listview->hideColumn( 2 );
    listview->setTreeStepSize(20);
    listview->setRootIsDecorated(true);

    listview->header()->moveSection(1, 0);
    listview->header()->setMovingEnabled ( 0 );
    listview->header()->setResizeEnabled ( 0 );
    listview->header()->setResizeEnabled ( 0 );

    listview->resize(404, 204);
    listview->setCaption("Tasker");
    listview->name("tasklist");
    a.name("tasklist");
    listview->setAllColumnsShowFocus(TRUE);
    listview->show();
    a.setMainWidget(listview);

//    server->start(host, options.port);

    a.exec();

    delete server;

    return 0;
}

#include <getopt.h>

static struct option long_options[] =
{
    {"debug", 0, NULL, 'D'},
    {"help", 0, NULL, '?'},
    {"iconify", 0, NULL, 'i'},
    {"port", 1, NULL, 'p'},
    {"server", 1, NULL, 's'},
    {"session", 1, NULL, 'n'},
    {"user", 1, NULL, 'u'},
    {"verbose", 0, NULL, 'v'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
};

#define _(x) x

void usage(char *name)
{
    fprintf(stderr, _("Usage: tlist [options]\n"
" Tasker display and control client\n"
"\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"\n"
" -D --debug                 stay in the foreground.\n"
" -s --server {ip address}   set servers ip address.\n"
" -p --port {port #}         set servers port number.\n"
" -u --user {username}       set user to something other than $USER.\n"
" -v --verbose               display more information.\n"
"\n"
" -? --help     display this help and exit\n"
" -V --version  output version information and exit\n"
"\n"
    ));
}

void parse_command_line(int argc, char **argv, struct cmdlineopt *opt)
{
    int c, i;

    opt->session = -1;
    while ((c = getopt_long(argc, argv, "d:D?ikP:p:s:u:v", long_options, NULL)) != -1) {
	switch (c) {
	    case 'd':
		if (optarg != NULL) {
		    opt->display = optarg;
		} else {
		    usage(argv[0]);
		    exit(0);
		}
		break;
	    case 'D':
		opt->debug = 1;
		break;
	    case 'i':
		opt->iconify = 1;
		break;
	    case '?':
		usage(argv[0]);
		exit(0);
	    case 'n':
		opt->session = atoi(optarg);
		break;
	    case 's':
		if (optarg != NULL) {
		    opt->server = optarg;
		} else {
		    usage(argv[0]);
		    exit(0);
		}
		break;
	    case 'u':
		if (optarg != NULL) {
		    opt->user = optarg;
		} else {
		    usage(argv[0]);
		    exit(0);
		}
		break;
	    case 'p':
		if (optarg != NULL) {
		    opt->port = atoi(optarg);
		} else {
		    usage(argv[0]);
		}
		break;
	    case 'V':
		printf("tlist (%s) %s\n", PACKAGE, VERSION);
		exit(0);
	    case 'v':
		opt->verbose++;
		break;
            default:
		printf("Try `%s --help' for more information.\n", argv[0]);
		exit(1);
	}
    }
}

/* eof main.cpp */
