/****************************************************************************
**
** Copyright (C) G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "ui_mainwindow.h"

class QAction;
class QTreeView;
class QWidget;
#include <QTime>
class QListViewItem;

class TTCP;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(TTCP *, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateActions();
    void set_current(int);

private slots:
    void insertChild();
    bool insertColumn(const QModelIndex &parent = QModelIndex());
    void insertRow();
    bool removeColumn(const QModelIndex &parent = QModelIndex());
    void removeRow();

    void add_entry(QString, int, int, QTime, QTime);

private:
    QHash<int, QListViewItem *> parents;
};

#endif

