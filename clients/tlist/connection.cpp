/****************************************************************************
**
** Copyright (C) 2008 G. Allen Morris III. All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <iostream>

#include "connection.h"

#include <QtNetwork>

#include "setup.h"

static const int PongTimeout =   5 * 1000;
static const int PingInterval =  2 * 1000;

using std::cerr;
using std::endl;

Connection::Connection(QObject *parent)
    : QSslSocket(parent)
{
    greetingMessage = tr("undefined");
    username = tr("unknown");
    cstate = WaitingForConnection;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(PingInterval);
    pongTimer.setInterval(PongTimeout);
    connectionTimer.setInterval(400);

    QObject::connect(this, SIGNAL(connected()),
                     this, SLOT(setupConnection()));

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    QObject::connect(&pongTimer, SIGNAL(timeout()), this, SLOT(setFailed()));

    QObject::connect(&connectionTimer, SIGNAL(timeout()),
                     this, SLOT(reConnect()));

    QObject::connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    QObject::connect(this, SIGNAL(disconnected()),
                     this, SLOT(setDisconnected()));

    QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(connectionError(QAbstractSocket::SocketError)));
}

QString Connection::name() const
{
    return username;
}

void Connection::setGreetingMessage(const QString &message)
{
std::cerr << "setGreetingMessage" << std::endl;
    greetingMessage = message;
}

bool Connection::sendMessage(const QString &message)
{
    if (message.isEmpty())
        return false;

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + " " + msg;
    return write(data) == data.size();
}

void Connection::timerEvent(QTimerEvent *timerEvent)
{
    if (timerEvent->timerId() == transferTimerId) {
        abort();
        killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

void Connection::sendAuthorize()
{
    QSettings settings("Tasker", "tlist");
    settings.beginGroup("Host");
    const QString host = settings.value("host", "127.0.0.1").toString();
    const qint16 port = settings.value("port", 8000).toInt();
    const bool ssl = settings.value("ssl", true).toBool();
    settings.endGroup();

    write(QByteArray("authorize\tgam3/testme\n"));
}

void Connection::processReadyRead()
{
    if (cstate == WaitingForGreeting) {
        if (!readProtocolHeader())
            return;
    } else 
    if (cstate == WaitingForAuthorized) {
        if (!readProtocolHeader())
	    return;
	cstate = ReadyForUse;
        emit readyForUse();
    } else
    if (cstate == ReadyForUse ) {
	while (canReadLine()) {
	    processData();
	};
    } else {
       exit(1); 
    }
}

void Connection::setFailed()
{
cerr << "fail" << endl;
}

void Connection::sendPing()
{
    if (!pongTimer.isActive()) {
        pongTimer.start();
    }
    write("ping\n");
}

void Connection::setDisconnected()
{
std::cerr << "Connection::setDisconnected" << std::endl << std::endl;
    cstate = Disconnected;
    connectionTimer.start();
    pingTimer.stop();
}

void Connection::setupConnection()
{
std::cerr << "Connection::setupConnection" << std::endl << std::endl;
    connectionTimer.stop();
    pingTimer.start();
    cstate = WaitingForGreeting;
}

bool Connection::readProtocolHeader()
{
    if (!canReadLine()) {
        return false;
    }
    if (transferTimerId) {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    QString line = readLine();
    line.remove('\n');
    line.remove('\r');

    QStringList list = line.split("\t");
    switch (cstate) {
	case WaitingForGreeting:
	    if (list[0] == "TTCP") {
		float version = list[1].toFloat(0);
		std::cerr << "version: " << version << std::endl;
		sendAuthorize();
		cstate = WaitingForAuthorized;
	    }
	    break;
	case WaitingForAuthorized:
	    if (list[0] == "notauthorized") {
		exit(1);
	    } else
	    if (list[0] == "authorized") {
		std::cerr << "authorized: " << std::endl;
		cstate = ReadyForUse;
	    } else {
cerr << qPrintable(line) << endl;
	    }
	    break;
        default:
	    break;
    }
    return true;
}

void Connection::processData()
{
    if (!canReadLine()) {
        return;
    }

    QString line = readLine();
    line.remove('\r');
    line.remove('\n');

    QStringList list = line.split("\t");

    pingTimer.start();

    if (list[0] == "pong") {
cerr << "pong" << endl;
        pongTimer.stop();
    } else {
        emit newCommand(list);
    }
}

void Connection::connectionError(QAbstractSocket::SocketError ername)
{
    if (ername == QAbstractSocket::RemoteHostClosedError) {
	std::cerr << "connectionError disconnect" << std::endl;
	connectionTimer.start();
    } else if (ername == QAbstractSocket::ConnectionRefusedError) { 
	connectionTimer.start();
    } else {
	std::cerr << "connectionError FATAL " << ername << std::endl;
    }
}

void Connection::reConnect()
{
    std::cerr << "reconnect" << std::endl;

    QSettings settings("Tasker", "tlist");
    settings.beginGroup("Host");
    const QString host = settings.value("host", "127.0.0.1").toString();
    const qint16 port = settings.value("port", 8000).toInt();
    const bool ssl = settings.value("ssl", true).toBool();
    settings.endGroup();

    connectionTimer.start();

    connectToHost(host, port);
}

/* eof */
