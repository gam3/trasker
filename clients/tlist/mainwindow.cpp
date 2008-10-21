/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#define Q_WS_X11

#include <iostream>

using std::cerr;
using std::endl;

#include <QtGui>

#include "mainwindow.h"
#include "notes.h"
#include "addproject.h"
#include "auto_select.h"
#include "error.h"
#include "timeedit.h"

#include "treemodel.h"
#include "treeitem.h"

class Project : public TreeItem {};

#include "ttcp.h"

MainWindow::MainWindow(TTCP *ttcp, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    readSettings();

    createActions();

    this->ttcp = ttcp;

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

    QStringList headers;
    headers << tr("Title") << tr("Description");

    TreeModel *model = new TreeModel(this);
    view->setModel(model);
    view->resizeColumnToContents(0);
    QHeaderView *header = view->header();
    header->moveSection(0, 1);
    view->setColumnWidth(1, 100);
    view->hideColumn(2);
    header->setMovable(false);

    /*! \sa
     * MyTreeView::popMenu()
     * MainWindow::itemMenu()
     */
    connect(ttcp, SIGNAL(error(const QString &)), this, SLOT(p_error(const QString &)));
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

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

#if defined (Q_WS_X11)
    x11();
#endif

    updateActions();

    addNoteW = new Notes(ttcp, this);
    connect(ttcp, SIGNAL(accept_note(const QString &)), addNoteW, SLOT(notesDone(const QString &)));
    addTaskW = new AddProject();
    addAutoSelW = new AddAuto(ttcp);
    errorWin = new ErrorWindow(this);
    timeEditWin = new TimeEdit(ttcp, this);
    
    connect(timeEditAction, SIGNAL(triggered()), timeEditWin, SLOT(myShow()));
}

#if defined (Q_WS_X11)
#include <X11/Xlib.h>
#include <QX11Info>

void MainWindow::x11()
{
    XSelectInput(QX11Info::display(), internalWinId(),
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
}
#endif

MainWindow::~MainWindow()
{
    std::cerr << "delete" << std::endl;
}

void MainWindow::insertChild()
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

bool MainWindow::insertColumn(const QModelIndex &parent)
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

void MainWindow::insertRow()
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

bool MainWindow::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::updateActions()
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
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void MainWindow::setCurrent(int id)
{
    if (id) {
	updateActions();
	TreeModel *model = (TreeModel *)view->model();
	TreeItem *item = model->getItem(id);

        if (item) {
	    statusBar()->showMessage(tr("Long.name.%1").arg(item->getName()));
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
    } else {
	statusBar()->showMessage(tr("No Project: (%1)").arg(id));
    }
}

void MainWindow::menu( const QModelIndex &/* index */ )
{
//    TreeModel *model = (TreeModel *)view->model();
    std::cerr << "mouse" << std::endl;
}

void MainWindow::myHide()
{
    saveSize = size();
    savePos = pos();
    hide();
}

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>

namespace x11 {
void wmMessage(Window win, long type, long l0, long l1, long l2, long l3, long l4);
}

void MainWindow::myShow()
{
    if (visible_flag) {
	std::cerr << "visible" << std::endl;
	return;
    }
    if (isVisible()) {
	activateWindow();
        const QX11Info x11Info;
	static Atom NET_ACTIVE_WINDOW = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
	x11::wmMessage(winId(), NET_ACTIVE_WINDOW, 2, CurrentTime, 0, 0, 0);
    } else {
	resize(saveSize);
	move(savePos);
	showNormal();
    }
}

void MainWindow::exposeCurrentProject()
{
    TreeModel *model = view->model();

    const QModelIndex &item = model->getCurrentIndex();

    show();

    view->scrollTo(item, QAbstractItemView::PositionAtCenter);
}

void MainWindow::createActions()
{
    selectCurrentAction = new QAction(tr("Select Current"), this);

    connect(selectCurrentAction, SIGNAL(triggered()), this, SLOT(exposeCurrentProject()));

    minimizeAction = new QAction(tr("Mi&nimize"), this);

    connect(minimizeAction, SIGNAL(triggered()),
            this, SLOT(myHide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(lower()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(myShow()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    startAction = new QAction(this);
    startAction->setText("&Start");
    startAction->setStatusTip(tr("Change to this project"));
    startAction->setShortcut(tr("Ctrl+S"));

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

    timeEditAction = new QAction(tr("Edit Times"), this);

    connect(timeEditAction, SIGNAL(triggered()), this, SLOT(timeEdit()));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
	if (visible()) {
	    visible_flag = false;
	    hidden_flag = true;
	    hide();
	} else {
	    myShow();
	}
        break;
    case QSystemTrayIcon::DoubleClick:
	std::cerr << "a" << std::endl;
        break;
    case QSystemTrayIcon::MiddleClick:
        {
	    QMenu *quickMenu = new QMenu;

	    int numRecentFiles = qMin(recent_projects.size(), 10);

            for (int i = 0; i < numRecentFiles; ++i) {
	        TreeItem *item = view->model()->getItem(recent_projects[i]);
		quickMenu->addAction(item->getName());
	    }

	    quickMenu->addSeparator();
	    quickMenu->addAction("Hello");
	    quickMenu->exec( QCursor::pos() );
	}
        break;
    default:
	std::cerr << "Mouse " << reason << std::endl;
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
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

void MainWindow::readSettings()
{
    QSettings settings("Tasker", "tlist");

    QPoint pos = settings.value("pos", QPoint(-1, -1)).toPoint();
    QSize size = settings.value("size", QSize(573, 468)).toSize();
    move(pos);
    savePos = pos;
    resize(size);
    saveSize = size;
}

void MainWindow::writeSettings()
{
    QSettings settings("Tasker", "tlist");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::x11Event(XEvent *event)
{
    switch (event->type) {
      case 15:
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
	      hidden_flag = true;
	      visible_flag = false;
	      break;
	}
	break;
      default:
	break;
    }
    
    return false;
}

bool MainWindow::visible()
{
    return visible_flag;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (1) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/*! Ask the server to change the current project
 */
void MainWindow::setProject(const QModelIndex &index)
{
    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    ttcp->setProject(item->getId());
}

void MainWindow::projItemMenu(int projId)
{
	QMenu bg_menu;

	bg_menu.addAction( startAction );
	bg_menu.addSeparator();
	bg_menu.addAction( noteAction );
	bg_menu.addAction( taskAction );
	bg_menu.addAction( autoAction );

	startAction->setData( QVariant(projId) );
	noteAction->setData( qVariantFromValue(projId) );
	taskAction->setData( qVariantFromValue(projId) );
	autoAction->setData( qVariantFromValue(projId) );

	bg_menu.exec( QCursor::pos() );
}

void MainWindow::itemMenu()
{
    QMenu popup;
    popup.addAction("bob");

    popup.exec( QCursor::pos() );
}

void MainWindow::startProject()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());
	std::cerr << projId << std::endl;
	emit changeProjectTo( projId );
    }
}

void MainWindow::p_error(const QString &error)
{
    errorWin->setText(error);
    errorWin->show();
}

void MainWindow::p_note()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());

	Project *projItem = getProject(projId);

	addNoteW->setProject(*projItem);
	addNoteW->show();
    }
}

void MainWindow::p_task()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());
	Project *projItem = getProject(projId);
	addTaskW->setProjectName(projItem->getName());
	addTaskW->show();
    }
}

void MainWindow::p_auto()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
	int projId =  qVariantValue<int>(action->data());

	ttcp->getauto(projId);

	Project *projItem = getProject(projId);

	addAutoSelW->setProjectName(projItem->getName());
	addAutoSelW->show();
    }
}

void MainWindow::timeEdit()
{
    cerr << "This is not used" << endl;
//   ttcp->gettime();
//    timeEditWin->show();
}

Project *MainWindow::getProject(int projId) {
    Project *proj = (Project *)view->model()->getItem(projId);
    return proj;
}

/* eof */
