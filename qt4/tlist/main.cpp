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

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
	QApplication::setQuitOnLastWindowClosed(false);
    }

    const QString host("127.0.0.1");
    const QString user("gam3");
    const QString password("ab12cd34");

    TTCP *ttcp = new TTCP(host, (quint16)8000, false, user, password);

    MainWindow window(ttcp);

    window.show();
    return app.exec();
}


/* eof */
