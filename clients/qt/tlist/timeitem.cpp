#include <QString>
#include <QStringList>
#include "timeitem.h"

TimeItem::TimeItem (TimeItem * parent)
{
    Q_UNUSED(parent);
}

TimeItem::TimeItem (QString user_in, int timeclice_id_in, int project_id,
		    int auto_id, QString from, QDateTime startTime_in,
                    QString duration_in, QDate currentDate)
 : id(timeclice_id_in), project_id(project_id), auto_id(auto_id), dateTime(startTime_in), elapsed(duration_in), displayDate(currentDate)
{
    Q_UNUSED(user_in);
    Q_UNUSED(from);
}

TimeItem::~TimeItem ()
{
}

//    QDateTime time;
QDateTime &TimeItem::datetime()
{
    return dateTime;
}

//    QString elapsed;
QString &TimeItem::elapsedData()
{
    return elapsed;
}

QString TimeItem::datetimeString()
{
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);

    if (duration_list[0].toInt() > 0) {
	return dateTime.toString("YY MMM dd");
    } else if (duration_list[2].toInt() && dateTime != dateTime) {
	return dateTime.toString("MMM dd");
    } else if (duration_list[2].toInt() && dateTime != dateTime) {
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

QDateTime TimeItem::endtime()
{
    // +year+months+days+hours+mins+secs
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);
    QDateTime endTime(dateTime);
    endTime = endTime.addYears(duration_list[0].toInt());
    endTime = endTime.addMonths(duration_list[1].toInt());
    endTime = endTime.addDays(duration_list[2].toInt());
    endTime = endTime.addSecs(duration_list[3].toInt() * 3600);
    endTime = endTime.addSecs(duration_list[4].toInt() * 60);
    endTime = endTime.addSecs(duration_list[5].toInt());
    return endTime;
}

QString TimeItem::endtimeString()
{
    // +year+months+days+hours+mins+secs
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);
    QDateTime endTime(dateTime);
    endTime = endTime.addYears(duration_list[0].toInt());
    endTime = endTime.addMonths(duration_list[1].toInt());
    endTime = endTime.addDays(duration_list[2].toInt());
    endTime = endTime.addSecs(duration_list[3].toInt() * 3600);
    endTime = endTime.addSecs(duration_list[4].toInt() * 60);
    endTime = endTime.addSecs(duration_list[5].toInt());
    return endTime.toString("hh:mm:ss");
}

QString TimeItem::project()
{
    return QString("%1").arg(project_id);
}

QString TimeItem::autoSelect()
{
    return QString("%1").arg(auto_id);
}

int &TimeItem::projectId()
{
    return project_id;
}

//int projectId() const
//{
//    return project_id;
//}

/* eof */
