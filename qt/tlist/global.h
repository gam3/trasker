
#ifndef GLOBAL_H
#define GLOBAL_H

struct cmdlineopt {
    int   session;
    int   debug;
    int   iconify;
    int   kill;
    char *previous_session_id;
    char *server;
    char *display;
    int   port;
    char *user;
    int   verbose;
};

#endif
