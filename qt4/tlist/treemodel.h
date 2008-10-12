/****************************************************************************
**
** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! \file treemodel.h
 *  \brief The project tree model
 *
 */

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash>

#include <QTime>
#include <QTimer>

class TreeItem;

class QMenu;
class QAction;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

signals:
    void get_time(int);

public slots:
    void set_current(int);
    void add_entry(QString,int,int,QTime,QTime);
    void expanded(const QModelIndex &item);
    void collapsed(const QModelIndex &item);
    void update_time(int id, QTime time, QTime atime);

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool setRow(const QModelIndex &index, const QString &name, int id, int pid, const QTime &time, const QTime &atime);

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());

    void itemMenu(const QModelIndex &);
    TreeItem *getItem(const int id) const;

    TreeItem *getCurrent();
    const QModelIndex getCurrentIndex();

private slots:
    void update_timer();

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);
    TreeItem *getItem(const QModelIndex &index) const;

    TreeItem *rootItem;
    TreeItem *currentItem;

    QHash<int, TreeItem *> parents;

    QTimer timer;
};

#endif

