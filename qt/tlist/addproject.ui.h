/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "addproject.moc"
#include "ttcp.h"

void AddProject::add_project()
{
    control->projectadd(user, parent_id, lineEdit1->text(), textEdit1->text()); 
}

void AddProject::accept_project(QString a)
{
    printf("%s\n", a.ascii());
    lineEdit1->clear();
    textEdit1->clear(); 
    accept();
}

void AddProject::setParent(QString user, const char *name, int id)
{
    Parent_Project->setText( name );
    parent_id = id;
    this->user = user;
}

void AddProject::setUser(QString user)
{
    this->user = user;
}

void AddProject::setControl( TTCP *control )
{
    this->control = control;

    QObject::connect(control, SIGNAL(accept_project(QString)),
		     this, SLOT(
			 accept_project(QString)
		     ));
}


void AddProject::newFunction()
{

}
