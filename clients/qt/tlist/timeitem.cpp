#include <QString>
#include <QStringList>
#include "timeitem.h"

TimeItem::TimeItem (TimeItem * parent)
{
    Q_UNUSED(parent);
}

TimeItem::TimeItem (QString user_in, int timeclice_id_in, int project_id,
		    int auto_id, QString from, QDateTime startTime_in,
                    QString duration_in, QDate currentDate) : id(timeclice_id_in), project_id(project_id), dateTime(startTime_in), elapsed(duration_in), displayDate(currentDate)
{
    Q_UNUSED(user_in);
    Q_UNUSED(auto_id);
    Q_UNUSED(from);
}

TimeItem::~TimeItem ()
{
}

//    QDateTime time;
//    QTime elapsed;
QString TimeItem::datetime()
{
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);
    if (duration_list[2].toInt() && dateTime != dateTime) {
	return dateTime.toString("MMM dd");
    } else {
	return dateTime.toString("hh:mm:ss");
    }
}

QString TimeItem::duration()
{
    QLatin1Char zed('0');
    QLatin1Char space(' ');
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);

    if (duration_list[2].toInt()) {
	return QString("Muiltiple Days");
    } else {
	return QString("+%1:%2:%3").arg(duration_list[3], 2, space)
				  .arg(duration_list[4], 2, zed)
				  .arg(duration_list[5], 2, zed);
    }
}

QString TimeItem::project()
{
    return QString("%1").arg(project_id);
}

