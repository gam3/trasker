/****************************************************************************
**
** Copyright (C) G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! \file connection.h
 *  \brief Connect to Tasker Server
 *
 *
 *  This controls the connection to the server and is
 *  used by ttcp.
 *
 *  This is a seperate object to handle a case where there
 *  are multiple servers.  Currently this is never the case.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QSslSocket>
#include <QTime>
#include <QTimer>

static const int MaxBufferSize = 1024000;

class Connection : public QSslSocket
{
    Q_OBJECT

public:
    enum ConnectionState {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };
    enum DataType {
        PlainText,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    Connection(QObject *parent = 0);

    QString name() const;
    void setGreetingMessage(const QString &message);
    bool sendMessage(const QString &message);

signals:
    void readyForUse();
    void newMessage(const QString &from, const QString &message);
    void newCommand(const QStringList &list);

protected:
    void timerEvent(QTimerEvent *timerEvent);

private slots:
    void processReadyRead();
    void sendPing();
    void sendGreetingMessage();
    void setDisconnected();
    void connectionError(QAbstractSocket::SocketError);
    void reConnect();

private:
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();

    QString greetingMessage;
    QString username;
    QTimer connectionTimer;
    QTimer pingTimer;
    QTime pongTime;
    QByteArray buffer;
    ConnectionState cstate;
    DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;
};

#endif
