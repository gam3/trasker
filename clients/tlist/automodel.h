/****************************************************************************
 **
 ** Copyright (C) 2006-2009 G. Allen Morris III, All rights reserved.
 **
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

/*! \file automodel.h
 *  \brief This is a list of autoSelect items
 *
 */

#ifndef AUTOMODEL_H
#define AUTOMODEL_H

#include <iostream>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash>

class AutoItem;

class QMenu;
class QAction;

class AutoModel:public QAbstractTableModel
{
    Q_OBJECT

    QVariant data(const QModelIndex & index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex())const;
    int columnCount(const QModelIndex & parent = QModelIndex())const;

public:
    AutoModel(QObject * parent = 0);
    ~AutoModel();

public slots:
    void add_autoentry(QString user, int autoselect_id, int project_id, QString host, QString name, QString className, QString role, QString desktop, QString title, QString flags);

private slots:

private:
    QList < int >ids;
    QHash < int, AutoItem * >timelist;
};
#endif
