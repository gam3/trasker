/****************************************************************************
**
** Copyright (C) G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! \file projects.h
 *  \brief The Main Window Object for tlist.
 *
 *  This is the window that lists all the available projects that
 *  that can be controled.
 */

#ifndef PROJECTS_H
#define PROJECTS_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSystemTrayIcon>

#include "ui_projects.h"


class QAction;
class QTreeView;
class QWidget;
#include <QTime>
class QListViewItem;

class Project;

class TTCP;
class Alerts;
class Notes;
class AddProject;
class AddAuto;
class ErrorWindow;
class TimeEdit;

#include <QMenu>

class MyPopupMenu : public QMenu {
    Q_OBJECT
  public:
    QListViewItem *Item;
};

/** This is the main window for the tlist program
 *
 */

class ProjectsTree : public QMainWindow, private Ui::ProjectsTree
{
    Q_OBJECT

public:
    ProjectsTree(TTCP *, QWidget *parent = 0);
    ~ProjectsTree();

    bool visible();

signals:
    void changeProjectTo(int);

public slots:
    void updateActions();
    //! Set the current project to
    void setCurrent(int);
    //! menu???
    void menu( const QModelIndex & index );
    void setProject(const QModelIndex & index);

    void projItemMenu(int projId);
    void itemMenu();

    void startProject();
    void p_error(const QString &);
    void p_note();
    void p_auto();
    void p_task();
    void timeEdit();

    void updateRecentMenu(QList<int>&);

private slots:
    void insertChild();
    bool insertColumn(const QModelIndex &parent = QModelIndex());
    void insertRow();
    bool removeColumn(const QModelIndex &parent = QModelIndex());
    void removeRow();

    void myHide();
    void myShow();
    void exposeCurrentProject();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();

#if defined (Q_WS_X11)
    void x11();
#endif

    MyPopupMenu it_menu;
    MyPopupMenu bg_menu;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QSize saveSize;
    QPoint savePos;

    void mousePressEvent(QMouseEvent *event);

    void readSettings();
    void writeSettings();

    void closeEvent(QCloseEvent *);
#if defined (Q_WS_X11)
    bool x11Event(XEvent *xe);
#endif
    bool hidden_flag;
    bool visible_flag;
    TTCP *ttcp;
    Alerts *alert;

private:
    QMenu *popup;

    QAction *startAction;
    QAction *noteAction;
    QAction *taskAction;
    QAction *autoAction;
    QAction *selectCurrentAction;
    QAction *timeEditAction;

    Notes *addNoteW;
    AddProject *addTaskW;
    AddAuto *addAutoSelW;
    ErrorWindow *errorWin;
    TimeEdit *timeEditWin;

    Project *getProject(int);

    /// A list of recently used projects
    QMenu *recentMenu;
    bool recentMenuClean;
    QList<int> recent_projects;
    QAction *recentProjectActions[10];
};

#endif

