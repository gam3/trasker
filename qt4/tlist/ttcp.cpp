
#include <QtNetwork>

#include <iostream>

#include "ttcp.h"
#include "connection.h"

TTCP::TTCP(const char *name, int port, bool ssl)
{
    connection = new Connection(this);

    newConnection(connection);

    QHostAddress senderIp("127.0.0.1");
    quint16 senderPort = 8000;

    connection->connectToHost(senderIp, senderPort);
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
    QObject::connect(connection, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(connectionError(QAbstractSocket::SocketError)));
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(connection, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    QObject::connect(connection, SIGNAL(newCommand(QStringList)), this, SLOT(newCommand(QStringList)));
    QObject::connect(connection, SIGNAL(connected()), this, SLOT(setConnected()));
    std::cerr << "TTCP::newConnection" << std::endl;
}

void TTCP::setunConnected()
{
    std::cerr << "TTCP::unconnected" << std::endl;
}
void TTCP::setConnected()
{
    std::cerr << "TTCP::connected" << std::endl;
}

void TTCP::readyForUse()
{
    Connection *connection = qobject_cast<Connection *>(sender());   // This is only needed if there is more than one connection

    connection->write("user gam3\n");
}

void TTCP::disconnected()
{
    std::cerr << "TTCP::disconnected" << std::endl;
}

void TTCP::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    std::cerr << "TTCP::connectionError" << std::endl;
}

QTime totime(QString timestring)
{
    QStringList tlist = timestring.split(":");
    QTime time(tlist[0].toInt(), tlist[1].toInt(), tlist[2].toInt());
    return time;
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
	emit error(list[2]);
    } else if (list[0] == "update") {
	emit update_all();
    } else if (list[0] == "update_time") {
	if (list.count() == 5) {
	    emit settime(list[2].toInt(), totime(list[3]), totime(list[4]));
	} else {
	    printf("Error in update_time\n");
	}
    } else if (list[0] == "accept_note") {
	emit accept_note(list[1]);
    } else if (list[0] == "accept_task") {
	emit accept_project(list[1]);
    } else if (list[0] == "accept_select") {
	emit accept_select(list[1]);
    } else {
	printf("TTCP Unknown %s/%d\n", qPrintable(list[0]), list.size() - 1);
    }
}

