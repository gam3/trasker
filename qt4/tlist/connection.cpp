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

static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 120 * 1000;
static const int PingInterval = 60 * 1000;
static const char SeparatorToken = ' ';

Connection::Connection(QObject *parent)
    : QSslSocket(parent)
{
    greetingMessage = tr("undefined");
    username = tr("unknown");
    state = WaitingForGreeting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(PingInterval);
    connectionTimer.setInterval(PingInterval);

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    QObject::connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    QObject::connect(&connectionTimer, SIGNAL(timeout()),
                     this, SLOT(reConnect()));
    QObject::connect(this, SIGNAL(disconnected()),
                     this, SLOT(setDisconnected()));
    QObject::connect(this, SIGNAL(connected()),
                     this, SLOT(sendGreetingMessage()));
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

void Connection::processReadyRead()
{
    if (state == WaitingForGreeting) {
std::cerr << "WaitingForGreeting" << std::endl;
        if (!readProtocolHeader())
            return;

        state = ReadingGreeting;
    }

    if (state == ReadingGreeting) {
        pingTimer.start();
        pongTime.start();
        state = ReadyForUse;
        emit readyForUse();
std::cerr << "ReadingGreeting" << std::endl;
    }

    while (canReadLine()) {
        processData();
    };
}

void Connection::sendPing()
{
    if (pongTime.elapsed() > PongTimeout) {
        abort();
        return;
    }

    write("ping\n");
}

void Connection::setDisconnected()
{
    std::cerr << "Disconnected!" << std::endl;
}

void Connection::sendGreetingMessage()
{
    std::cerr << "connected!" << std::endl;
    QByteArray data = "authorize\tgam3\tab12cd34\n";
    connectionTimer.stop();
    pingTimer.start();
    if (write(data) == data.size())
        isGreetingMessageSent = true;
}

int Connection::readDataIntoBuffer(int maxSize)
{
    if (maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = buffer.size();
    if (numBytesBeforeRead == MaxBufferSize) {
        abort();
        return 0;
    }

    while (bytesAvailable() > 0 && buffer.size() < maxSize) {
        buffer.append(read(1));
        if (buffer.endsWith(SeparatorToken))
            break;
    }
    return buffer.size() - numBytesBeforeRead;
}

int Connection::dataLengthForCurrentDataType()
{
    if (bytesAvailable() <= 0 || readDataIntoBuffer() <= 0
            || !buffer.endsWith(SeparatorToken))
        return 0;

    buffer.chop(1);
    int number = buffer.toInt();
    buffer.clear();
    return number;
}

bool Connection::readProtocolHeader()
{
    if (transferTimerId) {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    QString line = readLine();
    line.remove('\n');
    line.remove('\r');

    QStringList list = line.split("\t");
    if (list[0] == "TTCP") {
        float version = list[1].toFloat(0);
	std::cerr << "version: " << version << std::endl;
    }
    if (list[0] == "authorized") {
	std::cerr << "authorized: " << std::endl;
    }
    return true;
}

bool Connection::hasEnoughData()
{
    if (transferTimerId) {
        QObject::killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = dataLengthForCurrentDataType();

    if (bytesAvailable() < numBytesForCurrentDataType
            || numBytesForCurrentDataType <= 0) {
        transferTimerId = startTimer(TransferTimeout);
        return false;
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

    if (list[0] == "pong") {
        pongTime.start();
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
	std::cerr << "connectionError failed" << std::endl;
	connectionTimer.start();
    } else {
	std::cerr << "connectionError FATAL " << ername << std::endl;
    }
}

void Connection::reConnect()
{
    std::cerr << "reconnect" << std::endl;
    connectionTimer.start();

    QHostAddress senderIp("127.0.0.1");
    quint16 senderPort = 8000;

    connectToHost(senderIp, senderPort);
}

