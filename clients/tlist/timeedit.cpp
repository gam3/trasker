#include <iostream>

#include "timeedit.h"

#include "timemodel.h"
#include "timeitem.h"

#include <QTreeView>
#include <QHeaderView>

#include "ttcp.h"

TimeEdit::TimeEdit(TTCP * ttcp_in, QWidget * parent):QMainWindow(parent),
ttcp(ttcp_in)
{
    setupUi(this);

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
}

TimeEdit::~TimeEdit()
{

}

void TimeEdit::myShow()
{

    ttcp->getTimes();
    view->model()->revert();

    show();
}


/* eof */
