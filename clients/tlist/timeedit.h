/****************************************************************************
**
** Copyright (C) G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! \file timeedit.h
 *  \brief The Main Window Object for tlist.
 *
 *  This is the window that lists all the available projects that
 *  that can be controled.
 */

#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QMainWindow>
#include <QModelIndex>

#include "ui_timeedit.h"
#include "ttcp.h"

class TimeEdit:public QMainWindow, private Ui::TimeEdit {
    Q_OBJECT public slots:void myShow(void);
    void timeSlice(QString user,
		   int timeclice_id,
		   int project_id,
		   int auto_id,
		   QString from, QDateTime startTime, QTime duration);

  public:
    TimeEdit(TTCP * ttcp, QWidget * parent = 0);
    ~TimeEdit();

  protected:
    TTCP * ttcp;

};

#endif
