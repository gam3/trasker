
#include <QtNetwork>

#include <iostream>

#include "ttcp.h"
#include "connection.h"

TTCP::TTCP(const QString &host_in, quint16 port_in, bool ssl_in, const QString &user_in, const QString &password_in) : user(user_in), password(password_in), host(host_in), port(port_in), ssl(ssl_in)
{
    connection = new Connection(this);

    if (ssl) {
std::cerr << "SSL" << std::endl;
    }

    newConnection(connection);

    connection->reConnect();
}

TTCP::~TTCP()
{
    std::cerr << "delete TTCP" << std::endl;
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

void TTCP::addnote(const int id, const QString &note) const
{
    QString s(note);
    s.replace("\\", "\\\\");
    s.replace("\t", "\\t");
    s.replace("\n", "\\n");
    connection->write(QString("note\t%1\t%2\t%3\n").arg(user).arg(id).arg(s).toAscii());
}

void TTCP::setAuto(QString &host, QString &classN, QString &name, QString &role, QString &title, QString &desktop)
{

}

void TTCP::newCommand(const QStringList &list)
{
    if (list[0] == "entry") {
	emit add_entry(list[2], list[3].toInt(), list[4].toInt(), totime(list[5]),  totime(list[6]));
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
std::cerr << qPrintable(list[0]) << std::endl;
	emit accept_note(list[1]);
    } else if (list[0] == "accept_task") {
	emit accept_project(list[1]);
    } else if (list[0] == "accept_select") {
	emit accept_select(list[1]);
    } else {
	printf("TTCP Unknown: '%s'/%d\n", qPrintable(list[0]), list.size() - 1);
    }
}
