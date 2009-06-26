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

#include "auto_select.h"
#include "project.h"
#include "ttcp.h"

#include <QProcess>

AddAuto::AddAuto(TTCP *ttcp, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->ttcp = ttcp;
}

void AddAuto::setProject(const Project &proj)
{
    projectName->setText(proj.getName());
    project = &proj;
}

void AddAuto::setProjectName(const QString &name)
{
    projectName->setText(name);
}

void AddAuto::grabClicked()
{
    process = new QProcess(this);

    connect(process, SIGNAL(readyRead ()),
            this, SLOT(read()));

    process->start("winselect");

    process->waitForFinished();

    delete process;
    process = NULL;
}

void AddAuto::setAuto()
{
    int id = project->getId();

// Being checked causes a wild card.  Empty string is wildcard
//BUG you can't match an empty string!

    QString host(checkBox_host->isChecked() ? "" : Host->text());
    QString class_name(checkBox_class->isChecked() ? "" : Class->text());
    QString name(checkBox_name->isChecked() ? "" : Name->text());
    QString role(checkBox_role->isChecked() ? "" : Role->text());
    QString title(checkBox_title->isChecked() ? "" : Title->text());
    QString desktop(checkBox_desktop->isChecked() ? "" : Desktop->currentText());

    ttcp->addauto( id, host, class_name, name, role, title, desktop );
}

void AddAuto::buttonHelp_clicked()
{
    std::cerr << "help" << std::endl;	
}

void AddAuto::read()
{
    while (process->canReadLine()) {
        QString line = process->readLine(5000).constData();

	line.remove('\r');
	line.remove('\n');

	QStringList list = line.split(" ");

	if (list.size()) {
	    QString cmd = list.takeFirst();

	    if (cmd == "host") {
		Host->setText(list.join(" "));
	    } else
	    if (cmd == "name") {
		Name->setText(list.join(" "));
	    } else
	    if (cmd == "class") {
		Class->setText(list.join(" "));
	    } else
	    if (cmd == "role") {
		Role->setText(list.join(" "));
	    } else
	    if (cmd == "title") {
		Title->setText(list.join(" "));
	    } else
	    if (cmd == "WID") {
	    } else
	    if (cmd == "desktops") {
		Desktop->clear();
	    } else
	    if (cmd.startsWith("D")) {
	        QString x = cmd.right(1);
		Desktop->addItem(list.join(" "), x.toInt());
	    }
	}
    }
}

void AddAuto::autoDone(QString check)
{
    hide();
};


/* eof */
