/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "treeitem.h"

TreeItem::TreeItem(TreeItem *parent)
{
    parentItem = parent;
    this->id = 0;
    this->pid = 0;
}

TreeItem::TreeItem(const QString &name, const QTime &time, const QTime &atime, const int id, const int pid, TreeItem *parent)
{
    parentItem = parent;
    this->name = name;
    this->time = time;
    this->atime = atime;
    this->id = id;
    this->pid = pid;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number)
{
    return childItems.value(number);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return 2;
}

QVariant TreeItem::data(int column) const
{
    if (column == 0) {
        return name;
    } else {
       QString x;
       if (expanded_flag) {
	   return time.toString("hh:mm:ss");
       } else {
	   return atime.toString("hh:mm:ss");
       }
    }
}

TreeItem *TreeItem::appendChild(const QString &name, const int id, const int pid, const QTime &time, const QTime &atime)
{
    TreeItem *item = new TreeItem(name, time, atime, id, pid, this);
    childItems.insert(childItems.size(), item);

    return item;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        TreeItem *item = new TreeItem(this);
        childItems.insert(position, item);
    }

    return true;
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (TreeItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

void TreeItem::expanded(int flag)
{
    expanded_flag = flag; 
}

void TreeItem::start()
{
}

void TreeItem::stop()
{
}

#include <iostream>

QString TreeItem::getName() const
{
    return name;
}

int TreeItem::getId() const
{
    return id;
}

int TreeItem::getPid() const
{
    return pid;
}

void TreeItem::set_times(QTime time, QTime atime)
{
    this->time = time;
    this->atime = atime;
}

/* eof */
