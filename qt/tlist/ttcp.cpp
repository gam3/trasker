/****************************************************************************
** $Id: $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "ttcp.h"
#include <qurlinfo.h>
#include <stdlib.h>
#include <qurloperator.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "ttcp.moc"

TTCP::TTCP()
    : QNetworkProtocol(), connectionReady( FALSE ),
      readGroups( FALSE ), readArticle( FALSE ), timer(new QTimer), version(0.0)
{
    // create the command socket and connect to its signals
    commandSocket = new QSocket( this );
    connect( commandSocket, SIGNAL( hostFound() ),
             this, SLOT( hostFound() ) );
    connect( commandSocket, SIGNAL( connected() ),
             this, SLOT( myConnected() ) );
    connect( commandSocket, SIGNAL( connectionClosed() ),
             this, SLOT( closed() ) );
    connect( commandSocket, SIGNAL( readyRead() ),
             this, SLOT( readyRead() ) );
    connect( commandSocket, SIGNAL( error( int ) ),
             this, SLOT( error( int ) ) );
    connect( timer, SIGNAL( timeout() ),
             this, SLOT( timeout() ) );
    timer->start(10000);
}

TTCP::TTCP(const char *host, int port)
    : QNetworkProtocol(), connectionReady( FALSE ),
      readGroups( FALSE ), readArticle( FALSE ), timer(new QTimer), version(0.0)
{
    // create the command socket and connect to its signals
    commandSocket = new QSocket( this );
    connect( commandSocket, SIGNAL( hostFound() ),
             this, SLOT( hostFound() ) );
    connect( commandSocket, SIGNAL( connected() ),
             this, SLOT( myConnected() ) );
    connect( commandSocket, SIGNAL( connectionClosed() ),
             this, SLOT( closed() ) );
    connect( commandSocket, SIGNAL( readyRead() ),
             this, SLOT( readyRead() ) );
    connect( commandSocket, SIGNAL( error( int ) ),
             this, SLOT( error( int ) ) );
    connect( timer, SIGNAL( timeout() ),
             this, SLOT( timeout() ) );
    this->host = host;
    this->port = port;

    commandSocket->connectToHost(host, port);
    timer->start(10000);
}

TTCP::~TTCP()
{
    close();
    delete commandSocket;
}

void TTCP::operationListChildren( QNetworkOperation * )
{
    // create a command
    QString path = url()->path(), cmd;
    if ( path.isEmpty() || path == "/" ) {
        // if the path is empty or we are in the root dir,
        // we want to read the list of available newsgroups
        cmd = "list newsgroups\r\n";
    } else if ( url()->isDir() ) {
        // if the path is a directory (in our case a news group)
        // we want to list the articles of this group
        path = path.replace( "/", "" );
        cmd = "listgroup " + path + "\r\n";
    } else
        return;

    // write the command to the socket
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    readGroups = TRUE;
}

void TTCP::operationGet( QNetworkOperation *op )
{
    // get the dirPath of the URL (this is our news group)
    // and the filename (which is the article we want to read)
    QUrl u( op->arg( 0 ) );
    QString dirPath = u.dirPath(), file = u.fileName();
    dirPath = dirPath.replace( "/", "" );

    // go to the group in which the article is
    QString cmd;
    cmd = "group " + dirPath + "\r\n";
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );

    // read the head of the article
    cmd = "article " + file + "\r\n";
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    readArticle = TRUE;
}

bool TTCP::checkConnection( QNetworkOperation * )
{
    // we are connected, return TRUE
    if ( commandSocket->isOpen() && connectionReady )
        return TRUE;

    // seems that there is no chance to connect
    if ( commandSocket->isOpen() )
        return FALSE;

    // don't call connectToHost() if we are already trying to connect
    if ( commandSocket->state() == QSocket::Connecting )
        return FALSE;

    // start connecting
    connectionReady = FALSE;
    commandSocket->connectToHost( url()->host(),
                                  url()->port() != -1 ? url()->port() : 119 );
    return FALSE;
}

void TTCP::close()
{
    // close the command socket
    if ( commandSocket->isOpen() ) {
        commandSocket->writeBlock( "quit\r\n", strlen( "quit\r\n" ) );
        commandSocket->close();
    }
}

int TTCP::supportedOperations() const
{
    // we only support listing children and getting data
    return OpListChildren | OpGet;
}

void TTCP::hostFound()
{
    if ( url() )
        emit connectionStateChanged( ConHostFound, tr( "Host %1 found" ).arg( url()->host() ) );
    else
        emit connectionStateChanged( ConHostFound, tr( "Host found" ) );
}

void TTCP::myConnected()
{
    if ( host )
        emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( host ) );
    else
        emit connectionStateChanged( ConConnected, tr( "Connected to host" ) );
    emit connected();
}

void TTCP::closed()
{
    if ( url() )
        emit connectionStateChanged( ConClosed, tr( "Connection to %1 closed" ).arg( url()->host() ) );
    else
        emit connectionStateChanged( ConClosed, tr( "Connection closed" ) );
    version = 0.0;
    emit disconnected();
}

QTime totime(QString timestring)
{
    QStringList tlist = QStringList::split(":", timestring);
    QTime time(tlist[0].toInt(), tlist[1].toInt(), tlist[2].toInt());
    return time;
}

void TTCP::readyRead()
{
    while (commandSocket->canReadLine()) {
	QString line = commandSocket->readLine();
	line.remove('\n');
	QStringList list = QStringList::split("\t", line);

	if (0.0 == version) {
	    if (list[0] == "TTCP") {
		version = list[1].toFloat(0);
		return;
	    } else {
		printf("TTCP need version before %s\n", list[0].ascii());
		return;
	    }
	}

	if (list[0] == "entry") {
	    //         
	    emit entry(list[1].toInt(), list[2], list[3].toInt(), list[4].toInt(), totime(list[5]),  totime(list[6]));
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
	} else {
	    printf("TTCP Unknown %s\n", list[0].ascii());
	}
    }
}

void TTCP::error( int code )
{
    if ( code == QSocket::ErrHostNotFound ||
         code == QSocket::ErrConnectionRefused ) {
        // this signal is called if connecting to the server failed
        if ( operationInProgress() ) {
            QString msg = tr( "Host not found or couldn't connect to: \n" + url()->host() );
            operationInProgress()->setState( StFailed );
            operationInProgress()->setProtocolDetail( msg );
            operationInProgress()->setErrorCode( (int)ErrHostNotFound );
            clearOperationQueue();
            emit finished( operationInProgress() );
        }
    }
}

void TTCP::timeout()
{
    if (commandSocket->state() == QSocket::Idle) {
	commandSocket->connectToHost(host, port);
    }
}

void TTCP::get_entries( QString user )
{
    char buffer[100];
    sprintf(buffer, "user\t%s\n", user.ascii());
    commandSocket->writeBlock(buffer, strlen(buffer));
}

void TTCP::needtime( QString user, int project )
{
    char buffer[100];
    sprintf(buffer, "gettime\t%s\t%d\n", user.ascii(), project);
    commandSocket->writeBlock(buffer, strlen(buffer));
}

void TTCP::start( QString user, int project )
{
    char buffer[100];
    sprintf(buffer, "change\t%s\t%d\n", user.ascii(), project);
    commandSocket->writeBlock(buffer, strlen(buffer));
}

void TTCP::projectadd(QString user, const int parent, QString name, QString description )
{
    char buffer[100];

    sprintf(buffer, "addtask\t%s\t%d\t%s\t%s\n", user.ascii(), parent, name.ascii(), description.ascii());
    commandSocket->writeBlock(buffer, strlen(buffer));
}

void TTCP::noteadd( const char * user, const int project_id, const char *text )
{
    char buffer[1024];
    if (strlen(text) >= 1024) {
        return;
    }

    sprintf(buffer, "note\t%s\t%d\t%s\n", user, project_id, text);
    commandSocket->writeBlock(buffer, strlen(buffer));
}
/* eof */
