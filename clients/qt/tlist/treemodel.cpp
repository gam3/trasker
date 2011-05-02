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

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem();
    currentItem = NULL;
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(update_timer()));
    timer.setSingleShot(0);
    connect(&layout_timer, SIGNAL(timeout()),
            this, SIGNAL(layoutChanged()));
    layout_timer.setSingleShot(1);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

void TreeModel::expanded(const QModelIndex &index)
{
    TreeItem *item = getItem(index);
    item->expanded(1);
}

void TreeModel::collapsed(const QModelIndex &index)
{
    TreeItem *item = getItem(index);
    item->expanded(0);
}

int TreeModel::columnCount(const QModelIndex &/* parent */) const
{
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);

    switch (role) {
      case Qt::DisplayRole:
	return item->data(index.column());
      case Qt::StatusTipRole:
        return QString("StatusTip %1").arg("a");
      case Qt::ToolTipRole:
        return QString("%1").arg(item->getLongName());
      case Qt::DecorationRole:
	if (index.column() == 0) {
	    if (item == currentItem) {
	        return QIcon(":/pics/active-icon-0.xpm");
	    }
	}
        return QVariant();
      case Qt::BackgroundColorRole:
	if (index.column() == 0) {
	    if (item == currentItem) {
		return QColor(200,210,200);  // FIXME should be in config file
	    }
	}
	return QVariant();
      case Qt::TextColorRole:
      case Qt::FontRole:
      case Qt::TextAlignmentRole:
      case Qt::CheckStateRole:
      case Qt::SizeHintRole:
        return QVariant();
    }

    qWarning("Role: %d", role);
    return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable
    | Qt::ItemIsUserCheckable
//    | Qt::ItemIsDragEnabled
    | Qt::ItemIsEnabled
    ;
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

TreeItem *TreeModel::getItem(const int id) const
{
    TreeItem *item = parents[id];
    
    if (item) return item;
    return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
	    return QString("Project");
	} else {
	    return QString("Time");
	}
    }

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

bool TreeModel::hasChildren(const QModelIndex &index) const
{
    TreeItem *parentItem = getItem(index);

    return parentItem->childCount() > 0;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
    return false;
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);

    return item->setData(index.column(), value);
}

bool TreeModel::setRow(const QModelIndex &index, const QString &name, int id, int pid, const QTime &time, const QTime &atime)
{
    Q_UNUSED(index);
    Q_UNUSED(name);
    Q_UNUSED(id);
    Q_UNUSED(pid);
    Q_UNUSED(time);
    Q_UNUSED(atime);
    return false;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    return rootItem->setData(section, value);
}

void TreeModel::update_timer()
{
    static int x = 1;
    static int id = 1;
    const QTime n(0,0,0);
    if (currentItem) {
        if ((x % 59) == 0) {
	    while (id <= parents.size()) {
		TreeItem *item = parents[id++];
		// We only update times that are not zero;
		if (n < item->getTime()) {
		    emit get_time(item->getId());
		    break;
		}
	    }
	    if (id > parents.size()) id = 1;
	}
        if ((++x % 30) == 0) {
            for (TreeItem *item = currentItem; item; item = item->parent()) {
                if (item->getId()) {
                    emit get_time(item->getId());
                }
            }
        } else {
            currentItem->inc_time();
            for (TreeItem *item = currentItem; item; item = item->parent()) {
                if (item->getId()) {
                    item->inc_atime();
                    emit dataChanged(createIndex(0, 0, item), createIndex(0, 2, item));
                }
            }
        }
    }
}

void TreeModel::update_time(int id, QTime time, QTime atime)
{
    TreeItem *item = parents[id];
    if (item) {
	item->set_times(time, atime);
	emit dataChanged(createIndex(0, 0, item), createIndex(0, 2, item));
    }
}

TreeItem *TreeModel::getCurrent()
{
    return currentItem;
}

const QModelIndex TreeModel::getCurrentIndex()
{
    TreeItem *item = currentItem;

    if (item && item->parent()) {
	return createIndex(0, 0, item);
    }

    return QModelIndex();
}

void TreeModel::set_current(int id)
{
    TreeItem *item = parents[id];

    if (currentItem) {
        currentItem->stop();
    } else {
	timer.start(1000);
    }
    item->start();
    currentItem = item;
    emit dataChanged(createIndex(0, 0, item), createIndex(0, 1, item));
}

void TreeModel::add_entry(QString name, int id, int pid, const QTime time, const QTime atime)
{
    TreeItem *item;
    QModelIndex parentIndex;

    if (pid) {
	item = parents.value(pid);
        Q_ASSERT(item);
        parentIndex = createIndex(0, 0, item);
    } else {
        item = getItem(QModelIndex());
        parentIndex = QModelIndex();
    }

    if (parents[id]) {
       update_time(id, time, atime);
       qWarning("reload of \"entry\" (%d).", id);
       return;
    }

    int rows = rowCount(parentIndex); 
    beginInsertRows(parentIndex, rows, rows);
    if (TreeItem *child = item->appendChild(name, id, pid, time, atime)) {
        Q_ASSERT(child);
        parents[id] = child;
    }
    endInsertRows();

    layout_timer.start(1000);
//    emit layoutChanged();

    return;
}

/* eof */
