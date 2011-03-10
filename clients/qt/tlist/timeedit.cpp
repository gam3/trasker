#include <iostream>

#include "timeedit.h"

#include "timemodel.h"
#include "timeitem.h"

#include <QTreeView>
#include <QHeaderView>

#include "ttcp.h"

#include <QtDebug>
#include "timeeditdelegate.h"

TimeEdit::TimeEdit(TTCP * ttcp_in, QWidget * parent) : QMainWindow(parent), ttcp(ttcp_in)
{
    setupUi(this);

    dateEdit->setObjectName(QString::fromUtf8("dateEdit"));
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumDate(QDate(2000, 1, 1));
    dateEdit->setMaximumDate(QDate::currentDate());

    TimeModel *model = new TimeModel(this);
    view->setModel(model);

    view->setSortingEnabled(false);

    TimeEditDelegate *delegate = new TimeEditDelegate(this);
    view->setItemDelegate(delegate);

    view->setEditTriggers(QAbstractItemView::DoubleClicked
		          | QAbstractItemView::SelectedClicked);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->horizontalHeader()->setStretchLastSection(true);
    view->verticalHeader()->hide();

    view->show();

    connect(ttcp,
	    SIGNAL(add_timeslice
		   (QString, int, int, int, QString, QDateTime, QString)),
	    model,
	    SLOT(timeSlice
		 (QString, int, int, int, QString, QDateTime, QString)));

    connect(ttcp,
	    SIGNAL(add_entry
		   (QString,int,int,QTime,QTime)),
	    model,
	    SLOT(setProjectList(QString,int,int,QTime,QTime)));

    connect(ttcp,
	    SIGNAL(hourly()),
	    this,
	    SLOT(hourly()));

    connect(dateEdit,
	    SIGNAL(dateChanged(const QDate &)),
	    this,
	    SLOT(dateChanged(const QDate &)));

    connect(todayButton,
	    SIGNAL(clicked()),
	    this,
	    SLOT(setDateToday()));

    connect(refreshButton,
	    SIGNAL(clicked()),
	    this,
	    SLOT(refresh()));
}

TimeEdit::~TimeEdit()
{
    qWarning("TimeEdit::~TimeEdit");
}

// slot
void TimeEdit::dateChanged(const QDate &date)
{
    ((TimeModel *)view->model())->setDisplayDate(date);
    ttcp->getTimes(date);
    setWindowTitle(QString("Time Editor for %1").arg(date.toString("ddd MMMM d yyyy")));
}

// slot
void TimeEdit::setDateToday()
{
    QDate date = QDate::currentDate();
    ((TimeModel *)view->model())->setDisplayDate(date);
    ttcp->getTimes(date);
    setWindowTitle(QString("Time Editor for Today"));
}

// slot
void TimeEdit::refresh()
{
    ((TimeModel *)view->model())->setDisplayDate(dateEdit->date());
    ttcp->getTimes(dateEdit->date());
}

#if defined (Q_WS_X11)
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# include <QX11Info>

namespace x11 {
void wmMessage(Window win, long type, long l0, long l1, long l2, long l3, long l4);
}
#endif
// slot
void TimeEdit::myShow()
{
#if 0
    if (visible_flag) {
	return;
    }
#endif
    if (isVisible()) {
	activateWindow();
#if defined (Q_WS_X11)
        const QX11Info x11Info;
	static Atom NET_ACTIVE_WINDOW = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
	x11::wmMessage(winId(), NET_ACTIVE_WINDOW, 2, CurrentTime, 0, 0, 0);
#endif
    } else {
#if 0
	resize(saveSize);
	move(savePos);
#endif
	showNormal();

	((TimeModel *)view->model())->setDisplayDate(dateEdit->date());
	ttcp->getTimes(dateEdit->date());

	setWindowTitle(QString("Time Editor for %1").arg(dateEdit->date().toString("ddd MMMM d yyyy")));
    }
#if defined (Q_WS_X11)
    static Atom ATOM_WIN_STATE = XInternAtom(QX11Info::display(), "_WIN_STATE", False);
    x11::wmMessage(winId(), ATOM_WIN_STATE, 1, 1, CurrentTime, 0, 0);
#endif
}

void TimeEdit::setProjectList(QString name, int id, int pid, const QTime time, const QTime atime)
{
    Q_UNUSED(id);
    Q_UNUSED(pid);
    Q_UNUSED(time);
    Q_UNUSED(atime);
    qWarning("%s", qPrintable(name));
}

void TimeEdit::hourly()
{
    qWarning() << "timeedit hourly: " << QTime::currentTime().toString("h:mm:ss");
    //FIX this fails if we are in a different timezone
    dateEdit->setMaximumDate(QDate::currentDate());
}

/* eof */
