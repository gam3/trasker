/****************************************************************************
**
** Copyright (C) 2007-2011 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "mytableview.h"
#include "timeitem.h"

#include <QContextMenuEvent>
#include <QDebug>

MyTableView::MyTableView(QWidget *parent) : QTableView(parent)
{
//    setUniformRowHeights(true);
}

TimeModel *MyTableView::model() {
    return (TimeModel *)QTableView::model();
}

void MyTableView::contextMenuEvent(QContextMenuEvent *e)
{
    QModelIndex index = indexAt(e->pos());
    if (index.isValid()) {
	TimeItem *item = static_cast<TimeItem*>(index.internalPointer());
// qWarning() << "contextMenuEvent " << item << index.row() << index.column() << index.data(Qt::UserRole);
        emit timeslicePopMenu(index.data(Qt::UserRole).toInt());
    } else {
        emit popMenu();
    }
}

void MyTableView::keyPressEvent( QKeyEvent * event)
{
    QList<QAction *> bob;
    qWarning("C %d", event->key());
    bob = actions();
    qWarning("t: %d", bob.size());

    QAbstractItemView::keyPressEvent(event);
}

void MyTableView::enable()
{
    qWarning() << "enable";
    setEnabled(true);
}

void MyTableView::disable()
{
    qWarning() << "disable";
    setEnabled(false);
}

/* eof */
