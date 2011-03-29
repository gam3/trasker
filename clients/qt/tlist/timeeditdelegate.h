/****************************************************************************
**
** Copyright (C) 2011 G. Allen Morris III
** Contact: <gam3@gam3.net>
**
****************************************************************************/

#ifndef TIMEEDITDELEGATE_H
#define TIMEEDITDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>

#include "timemodel.h"

class TimeEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TimeEditDelegate(QObject *parent = 0, TimeModel *model = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    static bool isSupportedType(QVariant::Type type);
    static QString displayText(const QVariant &value);
#if 0
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;
#endif
private:
    mutable QRegExp timeExp;
    TimeModel *model;
};

#endif
