
#include "timeitem.h"

TimeItem::TimeItem (TimeItem * parent)
{
}

TimeItem::TimeItem (QString user_in, int timeclice_id_in, int project_id,
		    int auto_id, QString from, QDateTime startTime_in,
		    QString duration_in): id(timeclice_id_in), project_id(project_id), dateTime(startTime_in), elapsed(duration_in)
{
}

TimeItem::~TimeItem ()
{
}

//    QDateTime time;
//    QTime elapsed;
QString TimeItem::datetime()
{
    return dateTime.toString("yyyy-MM-dd hh:mm:ss");
}

QString TimeItem::duration()
{
    return elapsed;
}

QString TimeItem::project()
{
    return QString("%1").arg(project_id);
}

