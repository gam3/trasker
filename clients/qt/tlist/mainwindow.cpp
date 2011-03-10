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
#include <QSystemTrayIcon>

#include "mainwindow.h"

MainWindow::MainWindow(TTCP *ttcp, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    this->ttcp = ttcp;

#if !defined TRAYPROJECT
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);
    QIcon icon = QIcon(":/pics/active-icon-0.xpm");

    trayIconMenu->addAction(selectCurrentAction);
    trayIconMenu->addAction(timeEditAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setIcon(icon);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
#if 0
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
	    QMenu *recentMenu = new QMenu;
qWarning("MAIN");
            if (!recentMenuClean) {
		int numRecentFiles = qMin(recent_projects.size(), 10);

		for (int i = 0; i < numRecentFiles; ++i) {
		    TreeItem *item = view->model()->getItem(recent_projects[i]);
		    recentMenu->addAction(item->getName());
		}

		recentMenu->addSeparator();
		recentMenu->addAction("Hello");
            }
	    recentMenu->exec( QCursor::pos() );
	}
        break;
    case QSystemTrayIcon::Context:
        qWarning("Mouse main reason Right");
	break;
    default:
        qWarning("Mouse main reason %d", reason);
        break;
    }
#endif
}

/* eof */
