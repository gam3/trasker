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
    Q_OBJECT

  public:
    TimeEdit(TTCP * ttcp, QWidget * parent = 0);
    ~TimeEdit();

  protected:
    TTCP * ttcp;

public slots:
    void myShow(void);
    void dateChanged(const QDate & date);
    void setDateToday();
    void refresh(void);
    void hourly(void);  // update maxDate etc.
    void setProjectList(QString name, int id, int pid, const QTime time, const QTime atime); // add project information
};

#endif
