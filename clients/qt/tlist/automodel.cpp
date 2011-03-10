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
#include <QtDebug>

#include "automodel.h"
enum column {
  a_id,
  a_host,
  a_name,
  a_classname,
  a_role,
  a_title,
  a_desktop,
  a_project,
  a_enabled,
  a_max,
};

AutoModel::AutoModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

AutoModel::~AutoModel()
{
}

int AutoModel::rowCount(QModelIndex const& parent) const
{
    Q_UNUSED(parent);
    return autolist.size();
}

int AutoModel::columnCount(QModelIndex const&) const
{
    return a_max;
}

QVariant AutoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
	return QVariant();

    int id = ids[index.row()];
    AutoItem *item = autolist[id];
    if (role == Qt::EditRole) {
        qWarning("edit");
    }

    if (role == Qt::DisplayRole) {
	switch (index.column()) {
	  case a_id:
	    return item->id();
	  case a_host:
	    return item->host();
	  case a_name:
	    return item->name();
	  case a_classname:
	    return item->classname();
	  case a_role:
	    return item->role();
	  case a_title:
	    return item->title();
	  case a_desktop:
	    return item->desktop();
	  case a_project:
	    return item->project();
	  case a_enabled:
	    return item->flags();
	  default:
	    return item->project();
	}
    } else {
//        qWarning("%d", role);
    }
    return QVariant();
}

QVariant AutoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
	if (orientation == Qt::Horizontal) {
	    switch (section) {
	      case a_project:
                return QString("Project");
	      case a_host:
                return QString("Host");
	      case a_name:
                return QString("Name");
	      case a_classname:
                return QString("Class");
	      case a_role:
		return QString("Role");
	      case a_title:
		return QString("Title");
	      case a_desktop:
		return QString("Desktop");
	      case a_enabled:
		return QString("Flags");
	    }
	} else {
	    return QString("%1").arg(section);
	}
    }
    return QVariant();
}

void AutoModel::add_autoentry(QString user, int autoselect_id, int project_id,
                              QString host, QString b, QString c, QString d,
			      QString e, QString title, QString flags,
			      QString enabled)
{
    AutoItem *item;

    if (autolist.contains(autoselect_id)) {
qWarning("update auto?");
    } else {
	beginInsertRows(QModelIndex(), 0, ids.size());
	item = new AutoItem(user, autoselect_id, project_id, host, b, c, d, e, title, flags);
qWarning(qPrintable(enabled));
	autolist.insert(autoselect_id, item);
	ids.append(autoselect_id);
	endInsertRows();
	reset();
    }
}

