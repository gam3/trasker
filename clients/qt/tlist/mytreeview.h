/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/** This is a tree list view for Tasker
 *
 * There is one bug. You can't share a model between 2 views because the
 * model needs to know whether an entry is expanded or not.
 *
 * This is only need to overload the contextMenuEvent() method.
 */

#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>

//class QContextMenuEvent;

class TreeModel;


class MyTreeView : public QTreeView {
    Q_OBJECT

public slots:
    void enable();
    void disable();

signals:
    //! Request the global menu
    void popMenu();
    //! Request the project menu
    void projPopMenu(int);

public:
    MyTreeView(QWidget *parent = 0);
    //! The context menus
    /* 
     * This is the main way to control projects.
    */
    void contextMenuEvent(QContextMenuEvent *e);
    TreeModel *model();
};

#endif

/* eof */
