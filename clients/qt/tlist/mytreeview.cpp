/****************************************************************************
**
** Copyright (C) 2007-2011 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QContextMenuEvent>
#include "mytreeview.h"
#include "treeitem.h"

#include <QAction>

MyTreeView::MyTreeView(QWidget *parent) : QTreeView(parent)
{
    setUniformRowHeights(true);
    QAction bob("bob", this);
    QAction *bill = new QAction("bill", this);
    
    addAction(&bob);
    addAction(bill);
}

void MyTreeView::contextMenuEvent(QContextMenuEvent *e)
{
    QModelIndex index = indexAt(e->pos());

    if (index.isValid()) {
	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        emit projPopMenu(item->getId());
    } else {
        emit popMenu();
    }
}

TreeModel *MyTreeView::model() {
    return (TreeModel *)QTreeView::model();
}

void MyTreeView::keyPressEvent( QKeyEvent * event)
{
    QList<QAction *> bob;
    bob = actions();
qWarning("keyPressEvent");
    if (bob.size()) {
	qWarning("actions: %d", bob.size());
    }

    QAbstractItemView::keyPressEvent(event);
}

void MyTreeView::enable()
{
    setEnabled(true);
}

void MyTreeView::disable()
{
    setEnabled(false);
}

/* eof */
