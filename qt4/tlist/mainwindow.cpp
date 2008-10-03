/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include <QtGui>

#include "mainwindow.h"
#include "treemodel.h"

#include "ttcp.h"

MainWindow::MainWindow(TTCP *ttcp, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    QStringList headers;
    headers << tr("Title") << tr("Description");

    TreeModel *model = new TreeModel();

    view->setModel(model);
    view->resizeColumnToContents(0);
    QHeaderView *header = view->header();
    header->moveSection(0, 1);
    view->setColumnWidth(1, 100);
    view->hideColumn(2);
    header->setMovable(false);

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ttcp, SIGNAL(add_entry(QString,int,int,QTime,QTime)),
            this, SLOT(add_entry(QString,int,int,QTime,QTime)));

    connect(ttcp, SIGNAL(add_entry(QString,int,int,QTime,QTime)),
            model, SLOT(add_entry(QString,int,int,QTime,QTime)));

    connect(ttcp, SIGNAL(current(int)),
            this, SLOT(set_current(int)));

    connect(view->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));

    connect(actionsMenu, SIGNAL(aboutToShow()), this, SLOT(updateActions()));
    connect(insertRowAction, SIGNAL(triggered()), this, SLOT(insertRow()));
    connect(insertColumnAction, SIGNAL(triggered()), this, SLOT(insertColumn()));
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));
    connect(removeColumnAction, SIGNAL(triggered()), this, SLOT(removeColumn()));
    connect(insertChildAction, SIGNAL(triggered()), this, SLOT(insertChild()));

    updateActions();
}

MainWindow::~MainWindow()
{
    std::cerr << "delete" << std::endl;
}

void MainWindow::insertChild()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"),
                                 Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1, parent);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"),
                             Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool MainWindow::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void MainWindow::set_current(int id)
{
    if (id) {
	updateActions();
    }
}

void MainWindow::add_entry(QString name, int id, int pid, const QTime time, const QTime atime)
{
    return;
    if (pid > 0) {
    } else {
	QModelIndex index = view->rootIndex();
	QAbstractItemModel *model = view->model();

	if (model->columnCount(index) == 0) {
	    if (!model->insertColumn(0, index))
		return;
	}

	if (!model->insertRow(0, index))
	    return;

	QModelIndex child = model->index(0, 0, index);

//        TreeItem *x = model->getItem(index);

	model->setData(child, QVariant(name), Qt::EditRole);
	child = model->index(0, 1, index);
	model->setData(child, QVariant(atime.toString("hh:mm")), Qt::EditRole);
	child = model->index(0, 2, index);
	model->setData(child, QVariant(name), Qt::EditRole);

	updateActions();
    }
}

/* eof */
