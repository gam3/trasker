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

    if (role == Qt::BackgroundColorRole) {
	if (index.column() == 0) {
	    if (item == currentItem) {
		return QColor(200,210,200);
	    } else {
		return QVariant();
	    }
	} else {
	    return QVariant();
	}
    }
    if (role == Qt::TextColorRole)
	return QVariant();
    if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
	    if (item == currentItem) {
		return QIcon(":/pics/active-icon-0.xpm");
	    } else {
		return QVariant();
	    }
	} else {
	    return QVariant();
	}
    }
    if (role == Qt::FontRole)
	return QVariant();
    if (role == Qt::TextAlignmentRole)
	return QVariant();
    if (role == Qt::CheckStateRole)
	return QVariant();
    if (role == Qt::SizeHintRole)
	return QVariant();
    if (role == Qt::ToolTipRole)
	return QString("ToolTip %1").arg("a");
    if (role == Qt::StatusTipRole)
	return QString("StatusTip %1").arg("a");

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        //std::cerr << "Role: " << role << std::endl;
    }
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return item->data(index.column());
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

//! [7]
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
    if (currentItem) {
	for (TreeItem *item = currentItem; item; item = item->parent()) {
	    if (item->getId()) {
	        emit get_time(item->getId());
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
//FIXME
    if (parents[id]) {
       return;
    }

    int rows = rowCount(parentIndex); 
    beginInsertRows(parentIndex, rows, rows);
    if (TreeItem *child = item->appendChild(name, id, pid, time, atime)) {
        Q_ASSERT(child);
        parents[id] = child;
    }
    endInsertRows();

    return;
}

/* eof */
