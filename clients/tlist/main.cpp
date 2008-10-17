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
#include "settings.h"
#include "setup.h"

#include "cmdline.h"

/** Tlist 
 *
 */

using std::cout;
using std::endl;

int
main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);
    gengetopt_args_info args_info;

    QApplication app(argc, argv);

    if (cmdline_parser(argc, argv, &args_info) != 0)
    	exit(1);
    
    if (args_info.setup_given) {
	Setup().exec();
    }

    QSettings settings("Tasker", "tlist");
    settings.beginGroup("User");
    const QString user = settings.value("user", "").toString();
    const QString password = settings.value("password", "").toString();
    settings.endGroup();
    settings.beginGroup("Host");
    const QString host = settings.value("password", "127.0.0.1").toString();
    const qint16 port = settings.value("port", 8000).toInt();
    const bool ssl = settings.value("ssl", true).toBool();
    settings.endGroup();

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
	QApplication::setQuitOnLastWindowClosed(false);
    }

    TTCP *ttcp = new TTCP(host, port, ssl, user, password);

    MainWindow window(ttcp);

    window.show();
    return app.exec();
}

/* eof */
