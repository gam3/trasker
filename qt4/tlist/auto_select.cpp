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
#include "ttcp.h"

#include <QProcess>

AddAuto::AddAuto(TTCP *ttcp, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->ttcp = ttcp;
}

void AddAuto::setProjectName(const QString &name)
{
    Project->setText(name);
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

std::cerr << "bob" << std::endl;
}

void AddAuto::buttonHelp_clicked()
{
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

/* eof */
