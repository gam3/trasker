/****************************************************************************
**
** Copyright (C) 2007-2009 G. Allen Morris III, All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! 
 *
 * \author G. Allen Morris III
 */

#include "addproject.h"
#include "ttcp.h"

AddProject::AddProject(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
}

void AddProject::setProjectName(const QString &name)
{
    Parent_Project->setText("'" + name + "'");
}

void AddProject::add_project()
{
}

void AddProject::show_help()
{
}

/* eof */
