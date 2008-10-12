/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III. All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include <QtGui>
#include <QApplication>
#include <QSystemTrayIcon>

#include "mainwindow.h"
#include "ttcp.h"

/** Tlist 
 *
 */

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    QApplication app(argc, argv);

    QSettings settings("Tasker", "tlist");

    settings.beginGroup("security");

    const QString user = settings.value("user", "").toString();
    const QString password = settings.value("password", "").toString();
    const QString host = settings.value("password", "127.0.0.1").toString();
    const qint16 port = settings.value("port", 8000).toInt();
    const bool ssl = settings.value("ssl", true).toBool();

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
	QApplication::setQuitOnLastWindowClosed(false);
    }

    TTCP *ttcp = new TTCP(host, port, ssl, user, password);

    MainWindow window(ttcp);

    window.show();
    return app.exec();
}


/* eof */
