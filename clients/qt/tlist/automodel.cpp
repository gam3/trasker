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

//#include "timeitem.h"
#include "automodel.h"

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
    return ids.size();
}

int AutoModel::columnCount(QModelIndex const&) const
{
    return 4;
}

QVariant AutoModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    return QVariant();
}

void AutoModel::add_autoentry(QString user, int autoselect_id, int project_id, QString host, QString b, QString c, QString d, QString e, QString title, QString flags)
{
    Q_UNUSED(user);
    Q_UNUSED(autoselect_id);
    Q_UNUSED(project_id);

    Q_UNUSED(b);
    Q_UNUSED(c);
    Q_UNUSED(d);
    Q_UNUSED(e);
    Q_UNUSED(title);
    Q_UNUSED(flags);
    qWarning() << autoselect_id << ":" << host << " " << title << ":" << flags;
}
