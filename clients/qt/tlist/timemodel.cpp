/****************************************************************************
**
** Copyright (C) 2006-2011 G. Allen Morris III, All rights reserved.
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
    return c_max;
}

bool TimeModel::setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    qWarning("update %d", role);
    return false;
}

QVariant TimeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
	return QVariant();

    int id = ids[index.row()];
    TimeItem *item = timelist[id];
    if (role == Qt::EditRole) {
	switch (index.column()) {
	  case c_id:
	    return id;
	  case c_datetime:
	    return item->datetime();
	  case c_endtime:
	    return item->endtime();
	  case c_duration:
            return item->duration();
          case c_autoselect:
            return item->autoSelect();
          case c_project:
	    {
		int id = item->project().toInt();
		const QString *xproject = projects[id];
		return *(xproject);
	    }
	  default:
	    return QString("1234");
	}
    }
    if (role == Qt::DisplayRole) {
	switch (index.column()) {
	  case c_id:
	    return id;
	  case c_datetime:
	    return item->datetimeString();
	  case c_endtime:
	    return item->endtimeString();
	  case c_duration:
            return item->duration();
          case c_autoselect:
            return item->autoSelect();
          case c_project:
	    {
		int id = item->project().toInt();
		const QString *xproject = projects[id];
		return *(xproject);
	    }
	  default:
	    return QString("1234");
	}
    } else if (role == Qt::EditRole) {
        qWarning("EditRole");
    } else {
    }
    return QVariant();
}

QVariant TimeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
	if (orientation == Qt::Horizontal) {
	    switch (section) {
	      case c_id:
                return QString("Hidden");
	      case c_datetime:
                return QString("Started");
	      case c_duration:
                return QString("Duration");
	      case c_autoselect:
		return QString("Auto");
	      case c_project:
		return QString("Project");
	    }
	} else {
	    return QString("%1").arg(section);
	}
    }
    return QVariant();
}

void TimeModel::timeSlice(QString user,
                          int timeslice_id,
                          int project_id,
                          int auto_id,
                          QString from, QDateTime startTime, QString duration)
{
    TimeItem *item;

    beginInsertRows(QModelIndex(), ids.size(), ids.size() + 1);
    ids.append(timeslice_id);
    item = new TimeItem(user, timeslice_id, project_id, auto_id, from, startTime, duration, displayDate);

    timelist[timeslice_id] = item;
    endInsertRows();

    refreshTimer.start();
}


void TimeModel::setDisplayDate(QDate const& date)
{
    revert();
    displayDate = date;
}

Qt::ItemFlags TimeModel::flags( const QModelIndex& index ) const
{
    Qt::ItemFlags flag = Qt::NoItemFlags;
    switch (index.column()) {
      case c_datetime:
      case c_project:
        flag |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
	break;
      case c_id:
        break;
      default:
        flag |= Qt::ItemIsEnabled;
        break;
    }
    return flag;
}

QStringList TimeModel::getProjectList()
{
    QHashIterator<int, const QString *> i(projects);
    QStringList ret;
    while (i.hasNext()) {
	i.next();
	QString x(i.value()->toAscii());
	ret.append(x);
    }
    return ret;
}

void TimeModel::setProjectList(QString name, int id, int pid, const QTime time, const QTime atime)
{
    char buffer[20];
    Q_UNUSED(pid);
    Q_UNUSED(time);
    Q_UNUSED(atime);
    snprintf(buffer, 20, " (%d)", id);

    QString *project = new QString(name + buffer);
    if (projects.contains(id)) {
        qWarning("memory leak in TimeModel::setProjectList");
    }
    projects[id] = project;
}

/* eof */
