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
#include <QtDebug>

static const int PongTimeout =   5 * 1000;
static const int PingInterval =  2 * 1000;

Connection::Connection(QObject *parent)
    : QSslSocket(parent)
{
    greetingMessage = tr("undefined");
//    username = tr("unknown");
    cstate = WaitingForConnection;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(PingInterval);
    pongTimer.setInterval(PongTimeout);
    connectionTimer.setInterval(4000);

    QObject::connect(this, SIGNAL(connected()),
                     this, SLOT(setupConnection()));

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    QObject::connect(&pongTimer, SIGNAL(timeout()), this, SLOT(setFailed()));

    QObject::connect(&connectionTimer, SIGNAL(timeout()),
                     this, SLOT(reConnectHost()));

    QObject::connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    QObject::connect(this, SIGNAL(disconnected()),
                     this, SLOT(setDisconnected()));

    QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(connectionError(QAbstractSocket::SocketError)));

    QObject::connect(this, SIGNAL(sslErrors(const QList<QSslError> &)),
                     this, SLOT(mySslErrors(const QList<QSslError> &)));
}

void Connection::setAuthorize(const QString &name, const QString &password_in)
{
    this->username = name;
    this->password = password_in;
//FIXME reauthorize?
}

void Connection::setHost(const QString &host_in, const qint16 port_in, const bool ssl_in)
{
    host = host_in;
    port = port_in;
    ssl  = ssl_in;
qWarning() << host << ':' << port << ' ' << ssl_in;
//FIXME reconnect?
}

QString Connection::name() const
{
    return username;
}

void Connection::setGreetingMessage(const QString &message)
{
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
    write(QString("authorize\t%1/%2\n").arg(username).arg(password).toAscii());
}

void Connection::processReadyRead()
{
    if (cstate == WaitingForGreeting) {
        if (!readProtocolHeader())
	    qWarning() << "error WaitingForGreeting";
    } else 
    if (cstate == WaitingForAuthorized) {
        if (!readProtocolHeader())
	    qWarning() << "error WaitingForAuthorized";
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
    qWarning("Pong Time timeout");
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
    cstate = Disconnected;
//    connectionTimer.start();
    pingTimer.stop();
}

void Connection::setupConnection()
{
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
                qWarning("version: %f", version);
		sendAuthorize();
		cstate = WaitingForAuthorized;
	    }
	    break;
	case WaitingForAuthorized:
	    if (list[0] == "notauthorized") {
		emit notAuthorized();
                cstate = WaitingForAuthorized;
	    } else
	    if (list[0] == "authorized") {
		cstate = ReadyForUse;
		emit readyForUse();
	    } else {
                qWarning("Unknown response: %s", qPrintable(line));
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
        pongTimer.stop();
    } else {
        emit newCommand(list);
    }
}

void Connection::connectionError(QAbstractSocket::SocketError ername)
{
    if (ername == QAbstractSocket::RemoteHostClosedError) {
	connectionTimer.start();
    } else if (ername == QAbstractSocket::ConnectionRefusedError) { 
	connectionTimer.start();
    } else {
        qWarning("connection Error FATAL %d", ername);
        connectionTimer.stop();
        exit(1);
    }
}

void Connection::connectHost()
{
    reConnectHost();
    if (!waitForConnected(300000)) {
        qWarning() << errorString();
        qFatal("Could Not Connect");
        exit(1);
    }
}

void Connection::reConnectHost()
{
    qWarning("%s:%d (%s)", qPrintable(host), port, ssl?"SSL":"clear");
    connectionTimer.start();
    if (ssl) {
        connectToHostEncrypted(host, port);
    } else {
        connectToHost(host, port);
    }
}

void Connection::mySslErrors(const QList<QSslError> &errors)
{
    ignoreSslErrors();
    foreach (QSslError error, errors) {
///FIX this should be an alert.
qDebug() << "a " << error;
    }
}

/* eof */
