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

#include <iostream>

#include "addproject.h"
#include "ttcp.h"
#include "project.h"

AddProject::AddProject(const TTCP *ttcp_in, QWidget *parent) : QDialog(parent), ttcp(ttcp_in)
{
    setupUi(this);
}

void AddProject::setParentProject(const Project &parentProject_in)
{
    parentProject = &parentProject_in;
    Parent_Project->setText(parentProject->getName());
}

void AddProject::add_project()
{
    int id = parentProject->getId();
    QString name(projectName->text());
    QString desc(projectDescription->toPlainText());
    projectName->setText("");
    projectDescription->setPlainText("");
    if (name.size()) {
	ttcp->addtask(id, name, desc);
    }
}

void AddProject::show_help()
{
qWarning("HELP");
}

void AddProject::done(QString check)
{
    Q_UNUSED(check);
    hide();
};

/* eof */
