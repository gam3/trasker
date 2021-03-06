
#include <QtNetwork>

#include <iostream>

#include "ttcp.h"
#include "connection.h"

TTCP::TTCP(const QString &host_in, quint16 port_in, bool ssl_in, const QString &user_in, const QString &password_in)
    : user(user_in), password(password_in), host(host_in), port(port_in), ssl(ssl_in)
{
    readyForUseFlag = false;
    connection = new Connection(this);

    newConnection(connection);   // set up signals

    connection->setAuthorize(user, password);
    connection->setHost(host, port, ssl);

    connection->connectHost();
}

TTCP::~TTCP()
{
}

void TTCP::sendMessage(const QString &message)
{
    if (message.isEmpty())
        return;

//    connection->sendMessage(message);
}

QString TTCP::nickName() const
{
    return QString("gam3@gam3.net");
}

QString TTCP::getProject( int ) const
{
    return QString("Project Name Here");
}


bool TTCP::hasConnection(const QHostAddress &senderIp, int senderPort) const
{
    Q_UNUSED(senderIp);
    Q_UNUSED(senderPort);
    return true;
}

void TTCP::newConnection(Connection *connection)
{

    QObject::connect(connection, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(notReadyForUse()));
//    QObject::connect(connection, SIGNAL(readyForUse()), this, SIGNAL(connected()));
    QObject::connect(connection, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    QObject::connect(connection, SIGNAL(newCommand(QStringList)), this, SLOT(newCommand(QStringList)));
    QObject::connect(connection, SIGNAL(connected()), this, SLOT(setConnected()));
}

void TTCP::setunConnected()
{
}

void TTCP::setConnected()
{
}

void TTCP::notReadyForUse()
{
    readyForUseFlag = false;
    qWarning() << "Not ready";
}

void TTCP::readyForUse()
{
    Connection *connection = qobject_cast<Connection *>(sender());   // This is only needed if there is more than one connection

    connection->write(QString("user\t%1\n").arg(user).toAscii());

    readyForUseFlag = true;

    emit connected();
    qWarning() << "Ready";
}

bool TTCP::isReadyForUse() const
{
    return readyForUseFlag;
}

QTime totime(QString timestring)
{
    QStringList tlist = timestring.split(":");

    QTime time(tlist[0].toInt(), tlist[1].toInt(), tlist[2].toInt());
    return time;
}

QDateTime todatetime(QString timestring)
{
    QString x;
    if (timestring.indexOf('.') > 0) {
        x = timestring.left(timestring.indexOf('.'));
    } else {
        x = timestring;
    }
    QDateTime time = QDateTime::fromString(x, "yyyy-MM-dd hh:mm:ss" );

//  cerr << qPrintable(x) << " -> " << qPrintable(time.toString("yyyy-MM-dd hh:mm:ss.zzz")) << endl;

    return time;
}

void TTCP::getauto(const int id)
{
    QByteArray cmd;
    connection->write(QString("getauto %1 %2\n").arg(user).arg(id).toAscii());
}

void TTCP::gettime(const int id)
{
    if (isReadyForUse())
	connection->write(QString("gettime %1 %2\n").arg(user).arg(id).toAscii());
}

void TTCP::setProject(const int id)
{
    QByteArray cmd;

    if (isReadyForUse())
	connection->write(QString("change %1 %2\n").arg(user).arg(id).toAscii());
}

void TTCP::getTimes()
{
    if (isReadyForUse())
	connection->write(QString("times\t%1\n").arg(user).toAscii());
}

void TTCP::getTimes(const QDate date)
{
    if (isReadyForUse())
	connection->write(QString("times\t%1\t%2\n").arg(user).arg(date.toString(Qt::ISODate)).toAscii());
}

void TTCP::getTimes(const QDate, const QDate)
{
}

void TTCP::addnote(const int id, const QString &note) const
{
    QString s(note);
    qWarning("addnote ");
    s.replace("\\", "\\\\");
    s.replace("\t", "\\t");
    s.replace("\n", "\\n");
    connection->write(QString("note\t%1\t%2\t%3\n").arg(user).arg(id).arg(s).toAscii());
}

void TTCP::addtask(int parentId, const QString &name, const QString &desc) const
{
    QString d(desc);
    d.replace("\\", "\\\\");
    d.replace("\t", "\\t");
    d.replace("\n", "\\n");
    QString n(name);
    d.replace("\\", "\\\\");
    d.replace("\t", "\\t");
    d.replace("\n", "\\n");

    connection->write(QString("addtask\t%1\t%2\t%3\t%4\n").arg(user).arg(parentId).arg(n).arg(d).toAscii());
}

void TTCP::addauto(const int id, const QString &host, const QString &classN, const QString &name,
                   const QString &role, const QString &title, const QString &desktop) const
{
    connection->write(QString("addauto\t%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg( user ).arg( id ).arg(host).arg(classN).arg(name).arg(role).arg(title).arg(desktop).toAscii());
}

void TTCP::setAuto(QString &host, QString &classN, QString &name, QString &role, QString &title, QString &desktop)
{
    Q_UNUSED(host);
    Q_UNUSED(classN);
    Q_UNUSED(name);
    Q_UNUSED(role);
    Q_UNUSED(title);
    Q_UNUSED(desktop);
}

void TTCP::getRecentProjects()
{
    connection->write(QString("recentProjects\t%1\n").arg(user).toAscii());
}

//
// This slot will change the project associatied with a given timeslice.
//

void TTCP::timesliceChangeProject(const int id, const int new_pid, const int old_pid)
{
    QByteArray cmd;

    if (isReadyForUse())
	connection->write(QString("timeedit\t%1\t%2\t%3\t%4\n").arg(user).arg(id).arg(new_pid).arg(old_pid).toAscii());
}

void TTCP::timesliceChangeTime(const int id, const QDateTime &new_time, const QDateTime &old_time)
{
    QByteArray cmd;

    if (isReadyForUse()) {
        QString old_ts(old_time.toString(Qt::ISODate));
        QString new_ts(new_time.toString(Qt::ISODate));

	connection->write(QString("timemove\t%1\t%2\t%3\t%4\n").arg(user).arg(id).arg(new_ts).arg(old_ts).toAscii());
    }
}

void TTCP::timesliceSplitTime(const int id, const QDateTime &time)
{
    QByteArray cmd;

    if (isReadyForUse()) {
        QString ts(time.toString(Qt::ISODate));

	connection->write(QString("timesplit\t%1\t%2\t%3\n").arg(user).arg(id).arg(ts).toAscii());
    }
}

void TTCP::timesliceSplitTime(const int id)
{
    QByteArray cmd;

    if (isReadyForUse()) {
	connection->write(QString("timesplit\t%1\t%2\n").arg(user).arg(id).toAscii());
    }
}

/*
  We have a new comand from the server.
  'emit' a signal.
 */
void TTCP::newCommand(const QStringList &list)
{
    if (list[0] == "auto") {

	if (list.size() == 12) {
	    emit add_autoentry(list[1], list[2].toInt(), list[3].toInt(), list[4], list[5], list[6], list[7], list[8], list[9], list[10], list[11]);
			     /* user    project          auto_id          host     name     class    role     desk     title     flags    enabled */
	} else
	if (list.size() == 11) {
	    emit add_autoentry(list[1], list[2].toInt(), list[3].toInt(), list[4], list[5], list[6], list[7], list[8], list[9], list[10], "t");
			     /* user    project          auto_id          host     name     class    role     desk     title     flags    enabled */
	}
    } else if (list[0] == "entry") {
	emit add_entry(list[2], list[3].toInt(), list[4].toInt(), totime(list[5]),  totime(list[6]));
    } else if (list[0] == "timeslice") {
        if (list.size() == 8) {
            emit add_timeslice(list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5], todatetime(list[6]), list[7]);
        } else {
            qWarning("timeslice: wrong # of arguments %d", list.size());
	}
    } else if (list[0] == "current") {
	emit current(list[2].toInt());
    } else if (list[0] == "disable") {
	emit disable(list[2].toInt());
    } else if (list[0] == "error") {
	emit error(list[1]);
    } else if (list[0] == "update") {
	emit update_all();
    } else if (list[0] == "update_time") {
	if (list.count() == 5) {
	    emit settime(list[2].toInt(), totime(list[3]), totime(list[4]));
	} else {
	    printf("Error in update_time\n");
	}
    } else if (list[0] == "accept_note") {
        qWarning("%s %s", qPrintable(list[0]), qPrintable(list[1]));
	emit accept_note(list[1]);
    } else if (list[0] == "accept_task") {
	emit accept_project(list[1]);
    } else if (list[0] == "accept_select") {
	emit accept_select(list[1]);
    } else if (list[0] == "alert") {
        emit alert_message(list[1].toInt(), list[2], list[3]);
    } else if (list[0] == "alert_end") {
        emit alert_end_message(list[1].toInt());
    } else if (list[0] == "hourly") {
        qWarning("Hourly");
        emit hourly();
    } else if (list[0] == "recentproject") {
        emit recentproject(
                    list[2].toInt(),   // project ID
                    list[3].toInt(),   // index
                    list[4].toInt(),   // max index
                    QString("A comment")
                );
    } else if (list[0] == "recentprojects") {
        QStringList plist(list.mid(2));
        QList<int> pilist;
        QStringList::iterator i;
        for (i = plist.begin(); i != plist.end(); ++i)
            pilist.append((*i).toInt());
        emit recentprojects(
                    pilist
                );
    } else {
        qWarning("TTCP Unknown: '%s'/%d\n", qPrintable(list[0]), list.size() - 1);
    }
}

// eof
