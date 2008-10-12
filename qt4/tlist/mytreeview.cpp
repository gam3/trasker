/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "mytreeview.h"
#include "treeitem.h"

#include <QContextMenuEvent>

MyTreeView::MyTreeView(QWidget *parent) : QTreeView(parent)
{
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

void MyTreeView::enable()
{
    setEnabled(true);
}

void MyTreeView::disable()
{
    setEnabled(false);
}

/* eof */
