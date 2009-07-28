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

#include "alertdisplay.h"
#include "alerts.h"
#include "ttcp.h"


Alerts::Alerts(const TTCP *ttcp_in, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->ttcp = ttcp_in;

    connect(ttcp, SIGNAL(alert_message(int, const QString &, const QString &)),
            this, SLOT(add(int, const QString &, const QString &)));

    connect(ttcp, SIGNAL(alert_end_message(int)),
            this, SLOT(update(int)));

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Alerts::update(int x)
{
    Q_UNUSED(x);
    qWarning("done");
}

void Alerts::update()
{
//    qWarning("update");
}

void Alerts::add(int id, const QString title, const QString description)
{
    Q_UNUSED(id);
    qWarning("test");
    AlertDisplay *alert = new AlertDisplay(this);
    alert->setWindowTitle(title);
    alert->setText(description);
    CurrentAlert->addPage(alert);
    show();
///FIX    ttcp->addtask(id, name, desc);
}

void Alerts::show_help()
{
}

/* eof */
