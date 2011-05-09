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

/*! \brief Class that makes the connection to the Trasker Server

  Connection creates a connection to the Trasker server, monitors that server
  and will automatically reconnect if the connect is lost and regained.

  It also handles the authorization to the server and get the server version number.
  Once this class signals connected the a know set of commands are available.

  The connstructor must be supplied with:
   - Hostname/IP
   - Host Port #
   - SSL flag
   - authorization username
   - authorization passphrase

<a href="http://doc.qt.nokia.com/latest/qsslsocket.html">QSslSocket</a> 

 */

class Connection : public QSslSocket
{
    Q_OBJECT

public:
    /*! \brief The state of the connection
     The connection can be in any of the following states.
     ReadyForUse is the normal state. 
     */
    enum ConnectionState {
        WaitingForConnection,	/*!< Not connect to the server. */ 
	Disconnected,		/*!< */
        WaitingForGreeting,     /*!< waiting for server version information */
        WaitingForAuthorized,   /*!< Seny authorization waiting for approval */
        ReadyForUse		/*!< commands are emitted and methods send data */
    };
    //! Types of input data
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

    //! Send a message to the Trasker Server

    bool sendMessage(const QString &message);

    //! Set the login and password
    void setAuthorize(const QString &name, const QString &password);

    //! Change to host, port and ssl settings
    void setHost(const QString &host, const qint16 port, const bool ssl);

signals:
    //! The connection is ready for use.

    /*!
     The readyForUse() signal is emitted when the connection can be used.
     The version has been establish and the user has been authorized.

     */
    void readyForUse();
    void newMessage(const QString &from, const QString &message);
    void newCommand(const QStringList &list);

    //! Authorization failed
    void notAuthorized();

protected:
    void timerEvent(QTimerEvent *timerEvent);

public:
    //! connect to the server, exit on failure
    void connectHost();

public slots:
    //! connect to the server, return on failure
    void reConnectHost();

private slots:
    void processReadyRead();
    void sendPing();
    void setFailed();
    void setupConnection();
    void setDisconnected();
    void connectionError(QAbstractSocket::SocketError);
    void mySslErrors(const QList<QSslError> &errors);

private:
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();
    void sendAuthorize();

    QString greetingMessage;
    QTimer connectionTimer;
    QTimer pingTimer;
    QTimer pongTimer;
    QByteArray buffer;
    ConnectionState cstate;
    DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;

    bool ssl;
    QString host;
    qint16 port;

    QString username;
    QString password;
};

#endif
