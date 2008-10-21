#include <iostream>

using std::cerr;
using std::endl;

#include "timeedit.h"

#include "timemodel.h"
#include "timeitem.h"

#include "ttcp.h"

TimeEdit::TimeEdit(TTCP * ttcp_in, QWidget * parent):QMainWindow(parent),
ttcp(ttcp_in)
{
    setupUi(this);

    TimeModel *model = new TimeModel(this);
    view->setModel(model);

    connect(ttcp,
	    SIGNAL(add_timeslice
		   (QString, int, int, int, QString, QDateTime, QTime)),
	    model,
	    SLOT(timeSlice
		 (QString, int, int, int, QString, QDateTime, QTime)));
}

TimeEdit::~TimeEdit()
{
}

void TimeEdit::myShow()
{
    ttcp->getTimes();

    cerr << "TimeEdit::myShow()" << endl;

    show();
}

void TimeEdit::timeSlice(QString user,
			 int timeclice_id,
			 int project_id,
			 int auto_id,
			 QString from, QDateTime startTime, QTime duration)
{
    cerr << "got one" << endl;
}


/* eof */
