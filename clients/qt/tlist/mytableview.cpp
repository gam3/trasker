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

#include <QMenu>
#include <QAction>

MyTableView::MyTableView(QWidget *parent) : QTableView(parent)
{
}

void MyTableView::open()
{
    qWarning("open %d", ((QAction *)sender())->data().toInt());
    emit splitTimeslice(((QAction *)sender())->data().toInt());
}

void MyTableView::contextMenuEvent(QContextMenuEvent * e)
{
    QMenu *menu = new QMenu(this);
    QModelIndex index = indexAt(e->pos());
    if (index.isValid()) {
	TimeItem *item = static_cast<TimeItem*>(index.internalPointer());
        emit timeslicePopMenu(index.data(Qt::UserRole).toInt());

	QAction *split = new QAction(
	    QString("Row %1 - Col %2 was clicked on %3").arg(index.row()).arg(index.column()).arg(index.data(Qt::UserRole).toInt()),
	    menu);
	connect(split, SIGNAL(triggered()), this, SLOT(open()));
	split->setData(index.data(Qt::UserRole));
        menu->addAction(split);
    } else {
        menu->addAction("No item was clicked on");
    }
    menu->exec(QCursor::pos());
}

TimeModel *MyTableView::model() {
    return (TimeModel *)QTableView::model();
}

void MyTableView::keyPressEvent( QKeyEvent * event)
{
    QList<QAction *> bob;
    qWarning("MyTableView::keyPressEvent %d", event->key());
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
