#include <QString>
#include <QStringList>
#include "timeitem.h"

TimeItem::TimeItem (TimeItem * parent)
{
    Q_UNUSED(parent);
}

TimeItem::TimeItem (QString user_in, int timeclice_id_in, int project_id,
		    int auto_id, QString from, QDateTime startTime_in,
                    QString duration_in) : id(timeclice_id_in), project_id(project_id), dateTime(startTime_in), elapsed(duration_in)
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
    return dateTime.toString("hh:mm:ss");
}

QString TimeItem::duration()
{
    QLatin1Char zed('0');
    QStringList duration_list = elapsed.split("+", QString::SkipEmptyParts);
    return QString("%1:%2:%3").arg(duration_list[3], 2, zed)
                              .arg(duration_list[4], 2, zed)
			      .arg(duration_list[5], 2, zed);
}

QString TimeItem::project()
{
    return QString("%1").arg(project_id);
}

