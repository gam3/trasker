/*
 *
 */

#include <qlistview.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qsocket.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qheader.h>
#include <qdict.h>

#include <qaccel.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "addproject.h"
#include "notes.h"

#include "tlist.moc"

#include "tlist.h"

#include "ttcp.h"

TTListView::TTListView(QString user, TTCP *ttcp, int verb = 0) 
  : ttcp(ttcp), timer(new QTimer()), periodic(new QTimer()), verbose(verb)
{

    QObject::connect(ttcp, SIGNAL(entry(int, QString, int, int, QTime, QTime)),
		     this, SLOT(
			 entry(int, QString, int, int, QTime, QTime)
			 ));

    QObject::connect(ttcp, SIGNAL(current(int)),
		     this, SLOT(
			 current(int)
			 ));

    QObject::connect(ttcp, SIGNAL(settime(int, QTime, QTime)),
		     this, SLOT(
			 settime(int, QTime, QTime)
			 ));

    QObject::connect(ttcp, SIGNAL(update_all()),
		     this, SLOT(
			 update_all()
			 ));

    QObject::connect(ttcp, SIGNAL(connected()),
		     this, SLOT(
			 connected()
			 ));

    QObject::connect(ttcp, SIGNAL(disconnected()),
		     this, SLOT(
			 disconnected()
			 ));

    QObject::connect(timer, SIGNAL(timeout()),
		     this, SLOT(timeout()));

    QObject::connect(periodic, SIGNAL(timeout()),
		     this, SLOT(update()));

    QObject::connect(this, SIGNAL(expanded(QListViewItem *)),
		     this, SLOT(expanded(QListViewItem *)));

    QObject::connect(this, SIGNAL(collapsed(QListViewItem *)),
		     this, SLOT(collapsed(QListViewItem *)));

    current_project = 0L;
    time = QTime::currentTime(); 

    this->host = host;
    this->port = port;
    this->user = user;

    it_menu = new MyPopupMenu();
    it_menu->insertItem( "Start",  this, SLOT(p_start()), CTRL+Key_T );
    it_menu->insertItem( "Add Note",   this, SLOT(p_note()), CTRL+Key_N );
    it_menu->insertItem( "Add Task",   this, SLOT(p_add()), CTRL+Key_A );

    st_menu = new MyPopupMenu();
    st_menu->insertItem( "Add Note",   this, SLOT(p_note()), CTRL+Key_N );
    st_menu->insertItem( "Add Task",   this, SLOT(p_add()), CTRL+Key_A );

    bg_menu = new MyPopupMenu();
    bg_menu->insertItem( "Add Note",   this, SLOT(p_note()), CTRL+Key_N );
    bg_menu->insertItem( "Add Task",   this, SLOT(p_add()), CTRL+Key_A );

    add_proj = new AddProject();
    add_proj->setControl(ttcp);
    add_proj->setUser(user);

    add_note = new Notes();
    add_note->setControl(ttcp);
    add_note->setUser(user);

    QAccel *a = new QAccel( this );
    a->connectItem( a->insertItem(Key_N+CTRL),
	            this,
		    SLOT(p_note()) );

    periodic->start(3600 * 1000);
    timer->start(1000);
}

void TTListView::expanded(QListViewItem * item)
{
    QString sid = item->text(PROJECT_NUMBER);
    item->setText(PROJECT_TIME, Timers[sid]->toString("hh:mm"));
}

void TTListView::collapsed(QListViewItem * item)
{
    QString sid = item->text(PROJECT_NUMBER);
    item->setText(PROJECT_TIME, ATimers[sid]->toString("hh:mm"));
}

void TTListView::p_start()
{
    char buf[1024];
    QListViewItem *item;
    item = selectedItem();

    ttcp->start(user, item->text(PROJECT_NUMBER).toInt());
}

void TTListView::p_add()
{
    QListViewItem *item;
    item = selectedItem();

    assert(user.ascii());

    if (item) {
	add_proj->setParent(user, item->text(PROJECT_NAME).ascii(), item->text(PROJECT_NUMBER).toInt());
    } else {
	add_proj->setParent(user, tr( " -none- "), 0);
    }
    add_proj->show();
}

void TTListView::p_note()
{
    QListViewItem *item;
    item = selectedItem();

    if (item) {
	add_note->setProject(user, item->text(PROJECT_NAME).ascii(), item->text(PROJECT_NUMBER).toInt());
    } else {
	add_note->setProject(user, "All", 0);
    }
    add_note->show();
    add_note->setActiveWindow();
}

void TTListView::stop()
{
    char buf[1024];
    QListViewItem *item;
    item = selectedItem();

    sprintf(buf, "idle %s\n", user.ascii());
//    commandSocket->writeBlock(buf, strlen(buf));
}

void TTListView::slotRMB(QListViewItem * Item, const QPoint & point, int)
{
    if (Item)
	it_menu->popup( point );
    else
	bg_menu->popup( point );
}

void TTListView::slotMenu()
{
    exit(1);
}

/* This happens once per hour */
void TTListView::update()
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *last_item;
    long proj = current_project;

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	proj == item->text(PROJECT_NUMBER).toLong();
	ttcp->needtime(user, proj);
	++it;
    }
}

void TTListView::timeout()
{
    QListViewItem *item;

    if (item = get_current()) {
        QString sid = item->text(PROJECT_NUMBER);
	QTime *x;
	x = Timers[sid];
	*x = Timers[sid]->addSecs(1);
	x = ATimers[sid];
	*x = ATimers[sid]->addSecs(1);

	if (item->isOpen()) {
	    item->setText(PROJECT_TIME, Timers[sid]->toString("hh:mm:ss"));
	} else {
	    item->setText(PROJECT_TIME, ATimers[sid]->toString("hh:mm:ss"));
	}
	while (item = item->parent()) {
	    QString sid = item->text(PROJECT_NUMBER);
	    x = ATimers[sid];
	    *x = ATimers[sid]->addSecs(1);
	    if (!item->isOpen()) {
		item->setText(PROJECT_TIME, ATimers[sid]->toString("hh:mm:ss"));
	    }
	}
    }
}

void TTListView::disconnected()
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *last_item;
    long proj = current_project;

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	item->setEnabled(false);
	++it;
    }
//    this->setEnabled(false);
}

void TTListView::connected()
{
    printf("connected %s\n", user.ascii());

    ttcp->get_entries(user);
    this->setEnabled(true);
}

void TTListView::connection_error(int error)
{
    if (error != 0) {
	printf("Error %d\n", error);
    }
}

void TTListView::error(QString error_string)
{
    //
}


void TTListView::entry(int user_id, QString name, int project_id, int parent_id, QTime time, QTime atime)
{
    add_entry(name, project_id, parent_id, time, atime);
}

void TTListView::current(int project_id)
{
    set_currentN((long)project_id);
}

void TTListView::disable(int project_id)
{
    //
}

void TTListView::update_all()
{
    QListViewItemIterator it( this );

    while ( it.current() ) {
	QListViewItem *item = it.current();

        QString sid = item->text(PROJECT_NUMBER);

	ttcp->needtime(user, sid.toInt());

	++it;
    }
}

void TTListView::settime(int project_id, QTime time, QTime atime)
{
    QListViewItemIterator it( this );

    QTime *ntime = new QTime(time);
    QTime *natime = new QTime(atime);

    while ( it.current() ) {
	QListViewItem *item = it.current();
	if (project_id == item->text(PROJECT_NUMBER).toInt()) {
	    Timers.replace(item->text(PROJECT_NUMBER), ntime);
	    ATimers.replace(item->text(PROJECT_NUMBER), natime);

	    if (item->isOpen()) {
		item->setText(PROJECT_TIME, time.toString("hh:mm"));
	    } else {
		item->setText(PROJECT_TIME, atime.toString("hh:mm"));
	    }
	}
	++it;
    }
}

QListViewItem *TTListView::get_current()
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *last_item;
    long proj = current_project;

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	if (proj == item->text(PROJECT_NUMBER).toLong()) {
	    return item;
	}
	++it;
    }
    return NULL;
}

void TTListView::set_time(QString proj, QString time)
{
    QListViewItem *q;

    QStringList time_elements = QStringList::split(":", time);

    QTime *t = Timers[proj];
    if (time_elements.count() == 3) {
	t->setHMS(time_elements[0].toInt(), time_elements[1].toInt(), time_elements[2].toInt());
    }

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	if (proj.toLong() == item->text(PROJECT_NUMBER).toLong()) {
	    item->setText(PROJECT_TIME, t->toString("hh:mm"));
	}
	++it;
    }
}

void TTListView::set_currentN(long proj)
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *last_item;
    int old_project = current_project;

    if (current_project == proj) {
        return;
    }

    if (current_project && verbose) {
	printf("current %d -> %d\n", current_project, proj);
    }

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	if (proj == item->text(PROJECT_NUMBER).toLong()) {
	    setCaption(item->text(PROJECT_LONGNAME).ascii());
	    current_project = proj;
//	    ensureItemVisible(item);
	    setCurrentItem(item);
	}
	++it;
    }

    if (old_project) {
	ttcp->needtime(user, old_project);
    }
}

void TTListView::set_current(QString string)
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *last_item;

    QStringList list = QStringList::split(":", string);

    setCaption(string);

    if (item = selectedItem()) {
	item->setSelected(0);
    }
    item = this->firstChild();
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
	for (; item; item = item->nextSibling()) {
	    if (item->text(PROJECT_NAME) == *it) {
		item->setSelected(1);
		setCurrentItem(item);
		item = item->firstChild();
		last_item = item;
//		ensureItemVisible(item);
		break;
	    }
	}
    }
    if (last_item) {
    }
}

void TTListView::add_entry(QString name, int id, int pid, const QTime time, const QTime atime)
{
    QListViewItem *q;
    QListViewItem *item = this->firstChild();
    QListViewItem *parent = NULL;
    QString sid = QString::number(id, 10);
    QTime *ntime = new QTime(time);
    QTime *natime = new QTime(atime);

    setCaption(name);
    Timers.insert(sid, ntime);
    ATimers.insert(sid, natime);

    item = this->firstChild();

    QListViewItemIterator it( this );
    while ( it.current() ) {
	QListViewItem *item = it.current();
	if (pid && item->text(PROJECT_NUMBER).toInt() == pid) {
	    parent = item;
	} else
	if (item->text(PROJECT_NUMBER).toInt() == id) {
	    QListViewItem *new_parent = item->parent();
	    if (new_parent && parent) {
		assert(new_parent->text(PROJECT_NUMBER).toInt() == parent->text(PROJECT_NUMBER).toInt());
	    } else if (new_parent) {
		assert(0);
	    } else if (parent) {
		assert(0);
	    }

	    item->setEnabled(true);
	    item->setSelectable(true);
printf("%s %s (1)\n", item->text(PROJECT_NAME).ascii(), name.ascii());
// Fix need to update times and check parent
	    return;
	}
	++it;
    }

    if (parent) {
// FIX there is no long name now.
	item = new QListViewItem(parent, name, ATimers[sid]->toString("hh:mm"), sid, name);
    } else {
        if (!pid) {
	    item = new QListViewItem(this, name, ATimers[sid]->toString("hh:mm"), sid, name);
	} else {
	    assert(0);
	    return;
	}
    }
    item->setSelectable(1);

    return;
}

void TTListView::rightButtonPressed(QListViewItem * bob, const QPoint & x, int y)
{
    printf("ben\n");
}

/* eof */
