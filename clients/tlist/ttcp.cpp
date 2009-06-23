
#include <QtNetwork>

#include <iostream>

using std::cerr;
using std::endl;

#include "ttcp.h"
#include "connection.h"

TTCP::TTCP(const QString &host_in, quint16 port_in, bool ssl_in, const QString &user_in, const QString &password_in) : user(user_in), password(password_in), host(host_in), port(port_in), ssl(ssl_in)
{
    connection = new Connection(this);

    if (ssl) {
cerr << "SSL" << endl;
    }

    newConnection(connection);

    connection->reConnect();
}

TTCP::~TTCP()
{
    cerr << "delete TTCP" << endl;
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

bool TTCP::hasConnection(const QHostAddress &senderIp, int senderPort) const
{
    return true;
}

void TTCP::newConnection(Connection *connection)
{

    QObject::connect(connection, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(connection, SIGNAL(readyForUse()), this, SIGNAL(connected()));
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

void TTCP::readyForUse()
{
    Connection *connection = qobject_cast<Connection *>(sender());   // This is only needed if there is more than one connection

    connection->write("user gam3\n");
}

QTime totime(QString timestring)
{
    QStringList tlist = timestring.split(":");

    QTime time(tlist[0].toInt(), tlist[1].toInt(), tlist[2].toInt());
    return time;
}

QDateTime todatetime(QString timestring)
{
    QString x = timestring.left(timestring.indexOf('.')+4);

    QDateTime time = QDateTime::fromString(x, "yyyy-MM-dd hh:mm:ss.zzz" );

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
    if (connection->state() == QAbstractSocket::ConnectedState)
	connection->write(QString("gettime %1 %2\n").arg(user).arg(id).toAscii());
}

void TTCP::setProject(const int id)
{
    QByteArray cmd;

    if (connection->state() == QAbstractSocket::ConnectedState)
	connection->write(QString("change %1 %2\n").arg(user).arg(id).toAscii());
}

void TTCP::getTimes()
{
    if (connection->state() == QAbstractSocket::ConnectedState)
	connection->write(QString("times\t%1\n").arg(user).toAscii());
}
void TTCP::getTimes(const QDate)
{
}
void TTCP::getTimes(const QDate, const QDate)
{
}

void TTCP::addnote(const int id, const QString &note) const
{
    QString s(note);
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

void TTCP::addauto(const int id, const QString &host, const QString &classN, const QString &name, const QString &role, const QString &title, const QString &desktop) const
{

    cerr << "asdf" << endl;
    connection->write(QString("addauto\t%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(user).arg(id).arg(host).arg(classN).arg(name).arg(role).arg(title).arg(desktop).toAscii());
    cerr << "sdfa" << endl;
}

void TTCP::setAuto(QString &host, QString &classN, QString &name, QString &role, QString &title, QString &desktop)
{

}

void TTCP::newCommand(const QStringList &list)
{
    if (list[0] == "entry") {
	emit add_entry(list[2], list[3].toInt(), list[4].toInt(), totime(list[5]),  totime(list[6]));
    } else if (list[0] == "timeslice") {
        if (list.size() == 8) {
	    emit add_timeslice(list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5], todatetime(list[6]), list[7]);
	} else {
	    cerr << "timeslice: wrong # of arguments " << list.size() << endl;
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
cerr << qPrintable(list[0]) << endl;
	emit accept_note(list[1]);
    } else if (list[0] == "accept_task") {
	emit accept_project(list[1]);
    } else if (list[0] == "accept_select") {
cerr << qPrintable(list[0]) << " -> " << qPrintable(list[1]) << endl;
	emit accept_select(list[1]);
    } else {
	printf("TTCP Unknown: '%s'/%d\n", qPrintable(list[0]), list.size() - 1);
    }
}

