#ifndef TLIST_H
#define TLIST_H

#include <qlistview.h>
#include <qstring.h>
#include <qsocket.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qdict.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "addproject.h"
#include "notes.h"
#include "auto_select.h"

#include "ttcp.h"

#define PROJECT_NAME 0
#define PROJECT_TIME 1
#define PROJECT_NUMBER 2
#define PROJECT_LONGNAME 3
#define PROJECT_ATIME 4
// #define PROJECT_USER 1

class TTListView:public QListView {
    Q_OBJECT
private slots:
    void slotMenu();
    void set_current(QString);
    void set_currentN(long);
    void add_entry(QString, int, int, QTime, QTime);
//    void set_time(QString, QString);
    void update_all();
    void connected();
    void connection_error(int);
    void disconnected();
    void timeout();
    void update();
    void p_start();
    void p_add();
    void p_note();
    void p_select();
    void stop();
    QListViewItem *get_current();

  public:
//    TTListView::TTListView(QString user, QString host, int port, int verbose);
//    TTListView::TTListView(QString user, TTCP *server, int verbose);
    TTListView(QString user, QString host, int port, int verbose);
    TTListView(QString user, TTCP *server, int verbose);

  protected:
    QSocket *commandSocket;
    TTCP *ttcp;
    QString user;
    QPopupMenu* st_menu;
    QPopupMenu* it_menu;
    QPopupMenu* bg_menu;
    AddProject* add_proj;
    Notes* add_note;
    AutoSet *add_autoset;
    QTimer *timer;
    QTimer *periodic;
    long current_project;

    QDict<QTime> Timers;
    QDict<QTime> ATimers;
    QDict<int> expanded_flag;
    QDict<QListViewItem> parents;

    virtual void rightButtonPressed(QListViewItem *, const QPoint &, int);

  protected slots:
    void slotRMB(QListViewItem *, const QPoint &, int);
    void entry(int user_id, QString name, int project_id, int parent_id, QTime time, QTime atime);
    void current(int project_id);
    void disable(int project_id);
    void settime(int project_id, QTime time, QTime atime);
    void collapsed(QListViewItem * item);
    void expanded(QListViewItem * item);
    void error(QString error_string);
  private:
    QTime time;
    int verbose;
    int port;
    QString host;
    int tcount;
};

class MyPopupMenu:public QPopupMenu {
    Q_OBJECT
  public:
    QListViewItem *Item;
};

#endif
