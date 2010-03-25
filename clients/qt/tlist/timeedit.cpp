#include <iostream>

#include "timeedit.h"

#include "timemodel.h"
#include "timeitem.h"

#include <QTreeView>
#include <QHeaderView>

#include "ttcp.h"

#include <QtDebug>

TimeEdit::TimeEdit(TTCP * ttcp_in, QWidget * parent):QMainWindow(parent),
ttcp(ttcp_in)
{
    setupUi(this);

    dateEdit->setObjectName(QString::fromUtf8("dateEdit"));
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumDate(QDate(2000, 1, 1));
    dateEdit->setMaximumDate(QDate::currentDate());

    TimeModel *model = new TimeModel(this);
    view->setModel(model);

    view->setSortingEnabled(true);
//    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->horizontalHeader()->setStretchLastSection(true);
    view->verticalHeader()->hide();
    view->setEditTriggers(QAbstractItemView::SelectedClicked);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ttcp,
	    SIGNAL(add_timeslice
		   (QString, int, int, int, QString, QDateTime, QString)),
	    model,
	    SLOT(timeSlice
		 (QString, int, int, int, QString, QDateTime, QString)));

    connect(ttcp,
	    SIGNAL(hourly()),
	    this,
	    SLOT(hourly()));

    connect(dateEdit,
	    SIGNAL(dateChanged(const QDate &)),
	    this,
	    SLOT(dateChanged(const QDate &)));

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
void TimeEdit::refresh()
{
    ((TimeModel *)view->model())->setDisplayDate(dateEdit->date());
    ttcp->getTimes(dateEdit->date());
}

// slot
void TimeEdit::myShow()
{
    ((TimeModel *)view->model())->setDisplayDate(dateEdit->date());
    ttcp->getTimes(dateEdit->date());

    setWindowTitle(QString("Time Editor for %1").arg(dateEdit->date().toString("ddd MMMM d yyyy")));

    show();
}

void TimeEdit::hourly()
{
    qWarning() << "timeedit hourly: " << QTime::currentTime().toString("h:mm:ss");
    //FIX this fails if we are in a different timezone
    dateEdit->setMaximumDate(QDate::currentDate());
}

/* eof */
