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

#include <QtCore>
#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(TTCP *ttcp, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
}

MainWindow::~MainWindow()
{
}


/* eof */
