
#include <iostream>

#include "notes.h"
#include "ttcp.h"
#include "project.h"
#include "help.h"

Notes::Notes(const TTCP *ttcpN, QWidget *parent) : QDialog(parent), ttcp(ttcpN)
{
    setupUi(this);
}

void Notes::setProject(const Project &proj)
{
    projectName->setText(proj.getName());
    project = &proj;
}

void Notes::add_note()
{
    int id = project->getId();
    QString name(textEdit->toPlainText());
    textEdit->setPlainText("");
    if (name.size()) {
	ttcp->addnote(id, name);
    }
}

void Notes::show_help()
{
    HelpWin *win = new HelpWin();
    win->show();
}

void Notes::done(QString check)
{
    hide();
};

/* eof */
