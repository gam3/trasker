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

#include "notes.moc"
#include "ttcp.h"

void Notes::add_note()
{
    QString text = Note->text().replace(QChar('\n'), "\\n");
    control->noteadd(user.ascii(), id, text.ascii()); 
}

void Notes::accept_note(QString a)
{
    printf("Note message: %s\n", a.ascii());
    Project->clear();
    Note->clear(); 
    accept();
}

void Notes::setProject(QString user, const char *name, int id)
{
    Project->setText( name );
    this->id = id;
    this->user = user;
}

void Notes::setUser(QString user)
{
    this->user = user;
}

void Notes::setControl( TTCP *control )
{
    this->control = control;
    QObject::connect(control, SIGNAL(accept_note(QString)),
		     this, SLOT(
			 accept_note(QString)
		     ));
}
