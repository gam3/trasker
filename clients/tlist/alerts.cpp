/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*!
 *
 * \author G. Allen Morris III
 */

#include <iostream>

#include "alerts.h"
#include "ttcp.h"


Alerts::Alerts(const TTCP *ttcp_in, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->ttcp = ttcp_in;

    connect(ttcp, SIGNAL(alert_message(int, const QString &)),
            this, SLOT(add( const QString &)));

    connect(ttcp, SIGNAL(alert_end_message(int)),
            this, SLOT(update(int)));

    timer = new QTimer(this);http://www.facebook.com/n/?inbox/readmessage.php&t=1125024129045&mid=cb24bcG1fc778bfG2225565G0

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Alerts::update(int x)
{
    qWarning("done");
}

void Alerts::update()
{
    show();
    qWarning("update");
}

void Alerts::add(const QString text)
{
    qWarning("test");
    show();
///FIX    ttcp->addtask(id, name, desc);
}

void Alerts::show_help()
{
}

/* eof */
