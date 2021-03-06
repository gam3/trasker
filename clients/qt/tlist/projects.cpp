/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "conf.h"
#include <iostream>

#include <QtCore>
#include <QtGui>
#include <QtDebug>

#include "projects.h"
#include "notes.h"
#include "addproject.h"
#include "auto_select.h"
#include "error.h"
#include "timeedit.h"

#include "treemodel.h"
#include "treeitem.h"

class Project : public TreeItem {};

#include "ttcp.h"
#include "alerts.h"

ProjectsTree::ProjectsTree(TTCP *ttcp, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    setWindowFlags( windowFlags() & ~Qt::WindowMinimizeButtonHint );

    recentMenuClean = true;

    readSettings();

    recentMenu = new QMenu(this);
    alignmentGroup = new QActionGroup(this);

    recentMenu->setTitle(tr("projects"));
    recentMenu->setObjectName("project menu");
    recentMenu->setTearOffEnabled(true);

    TreeModel *model = new TreeModel(this);

    createActions();
    createViewActions();

    this->ttcp = ttcp;

#if defined TRAYPROJECT
    trayIcon = new QSystemTrayIcon();
    trayIconMenu = new QMenu();
    QIcon icon = QIcon(":/pics/active-icon-0.xpm");

    trayIconMenu->addAction(selectCurrentAction);
    trayIconMenu->addAction(timeEditAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setIcon(icon);
    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->show();
#endif

    QStringList headers;
    headers << tr("Title") << tr("Description");

    view->setModel(model);
    view->resizeColumnToContents(0);
    QHeaderView *header = view->header();
    header->moveSection(0, 1);
    view->setColumnWidth(1, 100);
    view->hideColumn(2);
    header->setMovable(false);

    /*! \sa
     * MyTreeView::popMenu()
     * ProjectsTree::itemMenu()
     */
    connect(ttcp, SIGNAL(error(const QString &)), this, SLOT(p_error(const QString &)));
    connect(ttcp, SIGNAL(recentprojects(QList<int>&)), this, SLOT(updateRecentMenu(QList<int>&)));

    connect(view, SIGNAL(popMenu()), this, SLOT(itemMenu()));
    connect(view, SIGNAL(projPopMenu(int)), this, SLOT(projItemMenu(int)));

    connect(model, SIGNAL(get_time(int)), ttcp, SLOT(gettime(int)));
    connect(this, SIGNAL(changeProjectTo(int)), ttcp, SLOT(setProject(int)));

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(view, SIGNAL(expanded(QModelIndex)),
            model, SLOT(expanded(QModelIndex)));

    connect(view, SIGNAL(collapsed(QModelIndex)),
            model, SLOT(collapsed(QModelIndex)));

    connect(ttcp, SIGNAL(disconnected()),
            view, SLOT(disable()));
    connect(ttcp, SIGNAL(connected()),
            view, SLOT(enable()));

    connect(ttcp, SIGNAL(add_entry(QString,int,int,QTime,QTime)),
            model, SLOT(add_entry(QString,int,int,QTime,QTime)));

    connect(ttcp, SIGNAL(current(int)),
            this, SLOT(setCurrent(int)));

    //! Unselect the view
    connect(ttcp, SIGNAL(current(int)),
            view, SLOT(clearSelection()));

    connect(ttcp, SIGNAL(current(int)),
            model, SLOT(set_current(int)));

    connect(ttcp, SIGNAL(settime(int, QTime, QTime)),
            model, SLOT(update_time(int, QTime, QTime)));

    connect(view->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));

#if defined TRAYPROJECT
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
#endif

#if defined (Q_WS_X11)
    x11();
#endif

    updateActions();

    addNoteW = new Notes(ttcp, this);
    connect(ttcp, SIGNAL(accept_note(const QString &)), addNoteW, SLOT(notesDone(const QString &)));
    addTaskW = new AddProject(ttcp, this);
    connect(ttcp, SIGNAL(accept_project(const QString &)), addTaskW, SLOT(done(const QString &)));
    addAutoSelW = new AddAuto(ttcp, this);
    connect(ttcp, SIGNAL(accept_select(const QString &)), addAutoSelW, SLOT(autoDone(const QString &)));
    errorWin = new ErrorWindow(this);
    timeEditWin = new TimeEdit(ttcp, this);
    
    connect(timeEditAction, SIGNAL(triggered()), timeEditWin, SLOT(myShow()));
//    timeEditWin->myShow();
}

#if defined (Q_WS_X11)
#include <X11/Xlib.h>
#include <QX11Info>

void ProjectsTree::x11()
{
    // follow the events listed here
    WId wid;
    if ((wid = winId())) {
	XSelectInput(QX11Info::display(), wid,
		     KeyPressMask | KeyReleaseMask |
		     ButtonPressMask | ButtonReleaseMask |
		     KeymapStateMask |
		     ButtonMotionMask | PointerMotionMask |
		     EnterWindowMask | LeaveWindowMask |
		     FocusChangeMask |
		     ExposureMask |
		     PropertyChangeMask |
		     StructureNotifyMask |
		     VisibilityChangeMask);
    } else {
qWarning("no window");
    }
}

#endif

ProjectsTree::~ProjectsTree()
{
}

void ProjectsTree::insertChild()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"),
                                 Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool ProjectsTree::insertColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1, parent);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"),
                             Qt::EditRole);

    updateActions();

    return changed;
}

void ProjectsTree::insertRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool ProjectsTree::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

void ProjectsTree::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

/* 

 */

void ProjectsTree::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid()) {
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        } else {
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
        }
    }
}

/*
 */

void ProjectsTree::setCurrent(int id)
{
    if (id) {
	current_project_id = id;
        updateActions();  //
	TreeModel *model = (TreeModel *)view->model();
	TreeItem *item = model->getItem(id);

        if (item) {
	    statusBar()->showMessage(tr("Long.name.%1").arg(item->getName()));
#if defined TRAYPROJECT
	    trayIcon->showMessage("Change", item->getName());
#endif
	} else {
	    statusBar()->showMessage(tr("Bad Project: (%1)").arg(id));
	}
	for (int i = 0; i < recent_projects.size(); ++i) {
	    if (recent_projects[i] == id) {
	        recent_projects.removeAt(i);
	    }
	}
	recent_projects.append(id);
	while (recent_projects.size() > 10) {
            recent_projects.removeFirst();
	}
	redrawRecentMenu();
    } else {
	statusBar()->showMessage(tr("No Project: (%1)").arg(id));
    }
}

void ProjectsTree::menu( const QModelIndex &/* index */ )
{
//    TreeModel *model = (TreeModel *)view->model();
    qWarning("mouse");
}

void ProjectsTree::myHide()
{
    saveSize = size();
    savePos = pos();
    visible_flag = false;
    hide();
}
#if defined (Q_WS_X11)
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# include <QX11Info>

namespace x11 {
void wmMessage(Window win, long type, long l0, long l1, long l2, long l3, long l4);
}
#endif
void ProjectsTree::myShow()
{
    if (visible_flag) {
	return;
    }
    if (isVisible()) {
	activateWindow();
#if defined (Q_WS_X11)
        const QX11Info x11Info;
	static Atom NET_ACTIVE_WINDOW = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
	x11::wmMessage(winId(), NET_ACTIVE_WINDOW, 2, CurrentTime, 0, 0, 0);
#endif
    } else {
	resize(saveSize);
	move(savePos);
	showNormal();
    }
#if defined (Q_WS_X11)
    static Atom ATOM_WIN_STATE = XInternAtom(QX11Info::display(), "_WIN_STATE", False);
    x11::wmMessage(winId(), ATOM_WIN_STATE, 1, 1, CurrentTime, 0, 0);
#endif
}

void ProjectsTree::exposeCurrentProject()
{
    TreeModel *model = view->model();

    const QModelIndex &item = model->getCurrentIndex();

    show();

    view->scrollTo(item, QAbstractItemView::PositionAtCenter);
}

void ProjectsTree::createActions()
{
    selectCurrentAction = new QAction(tr("Select Current"), this);
    selectCurrentAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_L);

    connect(selectCurrentAction, SIGNAL(triggered()), this, SLOT(exposeCurrentProject()));
    addAction(selectCurrentAction);

    minimizeAction = new QAction(tr("Mi&nimize"), this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(myHide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(lower()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(myShow()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    timeEditAction = new QAction(tr("Edit Times"), this);

    connect(timeEditAction, SIGNAL(triggered()), this, SLOT(timeEdit()));
}

void ProjectsTree::createViewActions()
{
    startAction = new QAction(this);
    startAction->setText("&Start");
    startAction->setStatusTip(tr("Change to this project"));
    startAction->setShortcut(tr("Ctrl+S"));
    startAction->setShortcutContext( Qt::ApplicationShortcut );

    connect(startAction, SIGNAL(triggered()), this, SLOT(startProject()));

    noteAction = new QAction(this);
    noteAction->setText("Add &Note");
    noteAction->setStatusTip(tr("Change to this project"));
    noteAction->setShortcut(tr("Ctrl+N"));

    connect(noteAction, SIGNAL(triggered()), this, SLOT(p_note()));

    taskAction = new QAction(this);
    taskAction->setText("Add &Task");
    taskAction->setStatusTip(tr("Add a task to this project"));
    taskAction->setShortcut(tr("Ctrl+T"));

    connect(taskAction, SIGNAL(triggered()), this, SLOT(p_task()));

    autoAction = new QAction(this);
    autoAction->setText("Add &Auto Select");
    autoAction->setStatusTip(tr("Set up auto select of project"));
    autoAction->setShortcut(tr("Ctrl+A"));

    connect(autoAction, SIGNAL(triggered()), this, SLOT(p_auto()));

    propAction = new QAction(this);
    propAction->setText("Prop&erities");
    propAction->setStatusTip(tr("Edit Properties of project"));
    propAction->setShortcut(tr("Ctrl+E"));

    connect(propAction, SIGNAL(triggered()), this, SLOT(p_prop()));
}

void ProjectsTree::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
	if (visible()) {
	    visible_flag = false;
	    hidden_flag = true;
            myHide();
	} else {
	    myShow();
	}
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        {
	    ttcp->getRecentProjects();
	}
        break;
    case QSystemTrayIcon::Context:
        break;
    default:
        qWarning("Mouse unknown reason %d", reason);
        break;
    }
}

void ProjectsTree::updateRecentMenu(QList<int> &list)
{
    QList<int>::iterator i;
    QList<int> temp;

    QList<QAction *> actions = recentMenu->actions();
    QList<QAction *>::iterator a;

    for (i = list.begin(); i != list.end(); ++i) {
        TreeItem *item = view->model()->getItem(*i);
        QAction *action = NULL;
	for (a = actions.begin(); a != actions.end(); ++a) {
	    if (((QAction *)*a)->data().toInt() == *i) {
                action = (QAction *)*a;
	    }
	}
	if (action == NULL) {
	    action = new QAction(item->getName(), recentMenu);
	    action->setToolTip("bob");
	    action->setData( QVariant(*i) );
	    action->setCheckable(true);
	    connect(action, SIGNAL(triggered()), this, SLOT(startProject()));
	    alignmentGroup->addAction(action);
	    recentMenu->addAction(action);
	}
	if (current_project_id == action->data().toInt()) {
	    action->setChecked(true);
	} else {
	    action->setChecked(false);
	}
    }
    recentMenu->exec( QCursor::pos() );
}

/* This updates the project quick menu */

void ProjectsTree::redrawRecentMenu()
{
    QList<QAction *>::iterator a;
    QList<QAction *> actions = recentMenu->actions();

    for (a = actions.begin(); a != actions.end(); ++a) {
	if (current_project_id == ((QAction *)*a)->data().toInt()) {
	    ((QAction *)*a)->setChecked(true);
	} else {
	    ((QAction *)*a)->setChecked(false);
	}
    }
}

void ProjectsTree::keyPressEvent(QKeyEvent *event)
{
    QList<QAction *> bob;
qWarning("ProjectsTree::keyPressEvent %d", event->key());
    bob = actions();
    qWarning("%d", bob.size());
    QWidget::keyPressEvent(event);
}

void ProjectsTree::mousePressEvent(QMouseEvent *event)
{
    QMenu quickMenu;

    quickMenu.addAction(selectCurrentAction);
    quickMenu.addAction(timeEditAction);
    quickMenu.addAction(minimizeAction);
    quickMenu.addAction(maximizeAction);
    quickMenu.addAction(restoreAction);
    quickMenu.addAction(quitAction);

    quickMenu.exec(event->globalPos(), restoreAction);
}

void ProjectsTree::readSettings()
{
    QSettings settings("Trasker", "tlist");

    settings.beginGroup("projectsTree");
    QPoint pos = settings.value("pos", QPoint(-1, -1)).toPoint();
    QSize size = settings.value("size", QSize(573, 468)).toSize();
    settings.endGroup();
    move(pos);
    savePos = pos;
    resize(size);
    saveSize = size;
}

void ProjectsTree::writeSettings()
{
    QSettings settings("Trasker", "tlist");
    settings.beginGroup("projectsTree");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    saveSize = size();
    savePos = pos();
}

#if defined (Q_WS_X11)
bool ProjectsTree::x11Event(XEvent *event)
{
    switch (event->type) {
qWarning("%d", event->type);
      case VisibilityNotify:
	switch(event->xvisibility.state) {
	   case 0:
	      visible_flag = true;
	      hidden_flag = false;
	      break;
	   case 1:
	      visible_flag = false;
	      hidden_flag = false;
	      break;
	   case 2:
	      visible_flag = false;
	      hidden_flag = true;
	      break;
	}
	break;
      default:
	break;
    }
    
    return false;
}
#endif
bool ProjectsTree::visible()
{
    return visible_flag;
}

void ProjectsTree::closeEvent(QCloseEvent *event)
{
    //cerr << "MainWindow::closeEvent" << endl;
    if (1) {
	visible_flag = false;
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/*! Ask the server to change the current project
 */
void ProjectsTree::setProject(const QModelIndex &index)
{
    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    ttcp->setProject(item->getId());
}

void ProjectsTree::projItemMenu(int projId)
{
    QMenu bg_menu;

    bg_menu.addSeparator();
    bg_menu.addAction( startAction );
    bg_menu.addSeparator();
    bg_menu.addAction( noteAction );
    bg_menu.addAction( taskAction );
    bg_menu.addAction( autoAction );
    bg_menu.addAction( propAction );

    startAction->setData( qVariantFromValue(projId) );
    noteAction->setData( qVariantFromValue(projId) );
    taskAction->setData( qVariantFromValue(projId) );
    autoAction->setData( qVariantFromValue(projId) );
    propAction->setData( qVariantFromValue(projId) );

    bg_menu.exec( QCursor::pos() );
}

void ProjectsTree::itemMenu()
{
    QMenu popup;
    popup.addAction("Add Project", this, SLOT(p_prop()));

    popup.exec( QCursor::pos() );
}

void ProjectsTree::startProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
qWarning("start");
    if (action) {
	int projId =  qVariantValue<int>(action->data());
        //std::cerr << projId << std::endl;
	emit changeProjectTo( projId );
    }
}

void ProjectsTree::p_error(const QString &error)
{
    errorWin->setText(error);
    errorWin->show();
}

void ProjectsTree::p_note()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());

	Project *projItem = getProject(projId);

	addNoteW->setProject(*projItem);
	addNoteW->show();
    }
}

void ProjectsTree::p_task()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());
	Project *projItem = getProject(projId);
	addTaskW->setParentProject(*projItem);
	addTaskW->show();
    }
}

void ProjectsTree::p_auto()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());

        ttcp->getauto(projId);  /*NOTE: it may be that this should be in the AutoSelect Object */

	Project *projItem = getProject(projId);

	addAutoSelW->setProjectName(projItem->getName());
	addAutoSelW->setProject(*projItem);
	addAutoSelW->show();
    }
}

void ProjectsTree::p_prop()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
//	int projId =  qVariantValue<int>(action->data());
    }
}


void ProjectsTree::timeEdit()
{
//    timeEditWin = new TimeEdit(ttcp, this);
    
//    connect(timeEditAction, SIGNAL(triggered()), timeEditWin, SLOT(myShow()));
    timeEditWin->myShow();
}

Project *ProjectsTree::getProject(int projId) {
    Project *proj = (Project *)view->model()->getItem(projId);
    return proj;
}

/* eof */
