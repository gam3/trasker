/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

#include <QTime>

class TreeItem
{
public:
    TreeItem(TreeItem *parent = 0);
    TreeItem(const QString &name, const QTime &time, const QTime &atime, const int id, const int pid, TreeItem *parent = 0);
    ~TreeItem();

    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    TreeItem *appendChild(const QString &name, const int, const int, const QTime&, const QTime&);
    bool insertColumns(int position, int columns);
    TreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);

    void start();
    void stop();

    void expanded(int);

    int getId() const;
    int getPid() const;
    QString getName() const;

    void set_times(QTime time, QTime atime);

private:
    QList<TreeItem*> childItems;
    TreeItem *parentItem;
    QVector<QVariant> itemData;
    QString name;
    int pid;
    int id;
    QTime time;
    QTime atime;
    QTime active;

    int expanded_flag;
};

#endif
