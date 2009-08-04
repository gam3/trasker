/****************************************************************************
 **
 ** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
 **
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

/*! \file timemodel.h
 *  \brief The project tree model
 *
 */

#ifndef TIMEMODEL_H
#define TIMEMODEL_H

#include <iostream>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash>

#include <QTime>
#include <QTimer>

class TimeItem;

class QMenu;
class QAction;

class TimeModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    TimeModel(QObject * parent = 0);
    ~TimeModel();

    QVariant data(const QModelIndex & index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;

    QModelIndex parent(const QModelIndex & index) const;

    int rowCount(const QModelIndex & parent = QModelIndex())const;
    int columnCount(const QModelIndex & parent = QModelIndex())const;

    bool setDate( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    Qt::ItemFlags flags( const QModelIndex& index ) const;

    void setDisplayDate(QDate const&);


public slots:
    void timeSlice(QString user,
      int timeclice_id,
      int project_id,
      int auto_id,
      QString from, QDateTime startTime,
      QString duration);

private slots:
    void revert();
    void myReset() {
        reset();
    }

private:
    QList < int >ids;
    QHash < int, TimeItem * >timelist;
    QTimer refreshTimer;

    QDate displayDate;
};
#endif
