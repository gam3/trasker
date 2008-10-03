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
    QApplication app(argc, argv);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
	QApplication::setQuitOnLastWindowClosed(false);
    }

    TTCP *ttcp = new TTCP("127.0.0.1", 8000, 0);

    MainWindow window(ttcp);
    window.show();

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon();
    trayIcon->show();

    return app.exec();
}


