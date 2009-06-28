/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include <QtGlobal>
#include <QtGui>

#include "timeitem.h"
#include "timemodel.h"

TimeModel::TimeModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    refreshTimer.setInterval(400);
    refreshTimer.setSingleShot(true);

    QObject::connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(myReset()));
}

TimeModel::~TimeModel()
{
}

void TimeModel::revert()
{
    ids.clear();
}

QModelIndex TimeModel::parent(QModelIndex const&) const
{
    return QModelIndex();
}

int TimeModel::rowCount(QModelIndex const& parent) const
{
    Q_UNUSED(parent);
    return ids.size();
}

int TimeModel::columnCount(QModelIndex const&) const
{
    return 4;
}

QVariant TimeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
	return QVariant();

    int id = ids[index.row()];
    TimeItem *item = timelist[id];

    if (role == Qt::DisplayRole) {
	switch (index.column()) {
	  case 0:
	    return id;
	  case 1:
	    return item->datetime();
	  case 2:
	    return item->duration();
	  case 3:
	    return item->project();
	  default:
	    return QString("1234");
	}
    }
    return QVariant();
}

QVariant TimeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
	if (orientation == Qt::Horizontal) {
	    switch (section) {
	      case 0:
		return QString("One");
	      case 1:
		return QString("Two");
	      case 2:
		return QString("Three");
	      case 3:
		return QString("Project");
	    }
	} else {
	    return QString("%1").arg(section);
	}
    }
    return QVariant();
}

void TimeModel::timeSlice(QString user,
			 int timeclice_id,
			 int project_id,
			 int auto_id,
			 QString from, QDateTime startTime, QString duration)
{
    TimeItem *item;


    beginInsertRows(QModelIndex(), 0, ids.size());
    ids.append(timeclice_id);
    item = new TimeItem(user, timeclice_id, project_id, auto_id, from, startTime, duration);
    timelist[timeclice_id] = item;

    endInsertRows();

    refreshTimer.start();
//    cerr << timeclice_id << ":" << ids.size() << endl;
}

