/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TIMEITEM_H
#define TIMEITEM_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QVector>

#include <QTime>

class TimeItem
{
public:
    TimeItem(TimeItem *parent = 0);
    TimeItem(const QString &name, const QTime &time, const QTime &atime, const int id, const int pid, TimeItem *parent = 0);
    TimeItem(QString user, int timeclice_id, int project_id, int auto_id, QString from, QDateTime startTime, QString duration, QDate displayDate);

    ~TimeItem();

    QString datetime(void);
    QString duration(void);
    QString project(void);
    QString autoSelect(void);

private:
    QString user;
    int id;
    int project_id;
    int auto_id;
    QDateTime dateTime;
    QString elapsed;
    QDate displayDate;
};

#endif
