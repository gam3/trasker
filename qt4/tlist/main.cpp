/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III. All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QSystemTrayIcon>

#include "mainwindow.h"
#include "ttcp.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    QApplication app(argc, argv);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
	QApplication::setQuitOnLastWindowClosed(false);
    }

    TTCP *ttcp = new TTCP("127.0.0.1", 8000, 0);

    MainWindow window(ttcp);
    window.show();

    QIcon icon = QIcon(":/pics/active-icon-0.xpm");

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon();

    QAction *maximizeAction = new QAction(QObject::tr("Ma&ximize"), trayIcon);
    QAction *minimizeAction = new QAction(QObject::tr("Mi&nimize"), trayIcon);

    QMenu *trayIconMenu = new QMenu();
    trayIconMenu->addAction(minimizeAction);

    trayIcon->setIcon(icon);
    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->show();

    return app.exec();
}

/* eof */
