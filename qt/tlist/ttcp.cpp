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
#include "base64.h"
#include <qurlinfo.h>
#include <stdlib.h>
#include <qurloperator.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qdom.h>

#include <qfile.h>

#include "ttcp.moc"

TTCP::TTCP(const char *_host, int _port, bool _ssl)
    : timer(new QTimer), version(0.0)
{
    // create the command socket and connect to its signals
    socket = new QSocket( this );
    connect( socket, SIGNAL( hostFound() ), this, SLOT( sock_hostFound() ) );
    connect( socket, SIGNAL( connected() ), this, SLOT( sock_connected() ) );
    connect( socket, SIGNAL( readyRead() ), this, SLOT( sock_readyRead() ) );
    connect( socket, SIGNAL( connectionClosed() ), this, SLOT( sock_closed() ) );
    connect( socket, SIGNAL( error( int ) ), this, SLOT( sock_error( int ) ) );

#if HAVE_QCA_H
    if (_ssl) {
	ssl = new QCA::TLS;
	connect(ssl, SIGNAL(handshaken()), SLOT(ssl_handshaken()));
	connect(ssl, SIGNAL(readyRead()), SLOT(ssl_readyRead()));
	connect(ssl, SIGNAL(readyReadOutgoing(int)), SLOT(ssl_readyReadOutgoing(int)));
	connect(ssl, SIGNAL(closed()), SLOT(ssl_closed()));
	connect(ssl, SIGNAL(error(int)), SLOT(ssl_error(int)));

	rootCerts.setAutoDelete(true);
	rootCerts = getRootCerts("/usr/share/psi/certs/rootcert.xml");
    } else {
        ssl = NULL;
    }
#endif

    host = _host; 
    port = _port;

    printf("Trying %s:%d...\n", host.latin1(), port);
    socket->connectToHost(host, port);

    timer->start(10000);

    connect( timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
}

TTCP::~TTCP()
{
    close();
    delete socket;
    if (ssl)
	delete ssl;
}

void TTCP::start(const char *_host, int _port)
{
    printf("starting\n");

    host = _host; 
    port = _port;

    socket->connectToHost(host, port);

    timer->start(10000);
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
    socket->writeBlock( cmd.latin1(), cmd.length() );

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
    socket->writeBlock( cmd.latin1(), cmd.length() );

    // read the head of the article
    cmd = "article " + file + "\r\n";
    socket->writeBlock( cmd.latin1(), cmd.length() );
    readArticle = TRUE;
}

bool TTCP::checkConnection( QNetworkOperation * )
{
    // we are connected, return TRUE
    if ( socket->isOpen() && connectionReady )
        return TRUE;

    // seems that there is no chance to connect
    if ( socket->isOpen() )
        return FALSE;

    // don't call connectToHost() if we are already trying to connect
    if ( socket->state() == QSocket::Connecting )
        return FALSE;

    // start connecting
    connectionReady = FALSE;
    socket->connectToHost( url()->host(),
                                  url()->port() != -1 ? url()->port() : 119 );
    return FALSE;
}

void TTCP::close()
{
    // close the command socket
    if ( socket->isOpen() ) {
        socket->writeBlock( "quit\r\n", strlen( "quit\r\n" ) );
        socket->close();
    }
}

int TTCP::supportedOperations() const
{
    // we only support listing children and getting data
    return OpListChildren | OpGet;
}

void TTCP::sock_hostFound()
{
printf("host found\n");
    if ( url() )
        emit connectionStateChanged( ConHostFound, tr( "Host %1 found" ).arg( url()->host() ) );
    else
        emit connectionStateChanged( ConHostFound, tr( "Host found" ) );
}

void TTCP::sock_connected()
{
    printf("Connected");

    if (ssl) {
	printf(", starting TLS handshake...");
	ssl->setCertificateStore(rootCerts);
	ssl->startClient(host);
    }
    printf("\n");

    if ( host )
        emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( host ) );
    else
        emit connectionStateChanged( ConConnected, tr( "Connected to host" ) );

    emit connected();
}

void TTCP::sock_closed()
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

void TTCP::doTheRead(QSocket *socket)
{
    while (socket->canReadLine()) {
	QString line = socket->readLine();
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
	} else if (list[0] == "accept_select") {
	    emit accept_select(list[1]);
	} else {
	    printf("TTCP Unknown %s\n", list[0].ascii());
	}
    }
}

void TTCP::sock_readyRead()
{
    if (ssl) {
	QByteArray buf(socket->bytesAvailable());
	int num = socket->readBlock(buf.data(), buf.size());
	if(num < (int)buf.size())
	    buf.resize(num);
	ssl->writeIncoming(buf);
    } else {
	while (socket->canReadLine()) {
	    QString line = socket->readLine();
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
	    } else if (list[0] == "accept_select") {
		emit accept_select(list[1]);
	    } else {
		printf("TTCP Unknown %s\n", list[0].ascii());
	    }
	}
    }
}

void TTCP::sock_error( int code )
{
    printf("sock error %d\n", code);
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
    } else {
	printf("unknown code: %d\n", code);
    }
}

void TTCP::timeout()
{
    if (socket->state() == QSocket::Idle) {
printf("re-connect\n");
printf("re-connect %s %d\n", host.ascii(), port);
printf("re-connect\n");
	socket->connectToHost(host, port);
    }
}

void TTCP::get_entries( QString user )
{
    char buffer[100];
    sprintf(buffer, "user\t%s\n", user.ascii());
    socket->writeBlock(buffer, strlen(buffer));
}

void TTCP::needtime( QString user, int project )
{
    char buffer[100];
    sprintf(buffer, "gettime\t%s\t%d\n", user.ascii(), project);
    socket->writeBlock(buffer, strlen(buffer));
}

void TTCP::start( QString user, int project )
{
    char buffer[100];
    sprintf(buffer, "change\t%s\t%d\n", user.ascii(), project);
    socket->writeBlock(buffer, strlen(buffer));
}

void TTCP::projectadd(QString user, const int parent, QString name, QString description )
{
    char buffer[100];

    sprintf(buffer, "addtask\t%s\t%d\t%s\t%s\n", user.ascii(), parent, name.ascii(), description.ascii());
    socket->writeBlock(buffer, strlen(buffer));
}

void TTCP::noteadd( const char * user, const int project_id, const char *text )
{
    char buffer[1024];
    if (strlen(text) >= 1024) {
        return;
    }

    sprintf(buffer, "note\t%s\t%d\t%s\n", user, project_id, text);
    socket->writeBlock(buffer, strlen(buffer));
}

void TTCP::setauto( const char * user, const int project_id,
                    QString host, QString name, QString Class,
		    QString title, QString role, QString desktop)
{
    char buffer[1024];

    sprintf(buffer,
"addauto\t%s\t%d"
"\t%s"  // host
"\t%s"  // name
"\t%s"  // class
"\t%s"  // title
"\t%s"  // role
"\t%s\n", // desktop
user, project_id,
host.ascii(),
name.ascii(),
Class.ascii(),
title.ascii(),
role.ascii(),
desktop.ascii());

    socket->writeBlock(buffer, strlen(buffer));
}

#if HAVE_QCA_H
void TTCP::ssl_handshaken()
{
    cert = ssl->peerCertificate();
    int vr = ssl->certificateValidityResult();

    printf("Successful SSL handshake.\n");
    if (!cert.isNull()) {
        showCertInfo(cert);
        printf("error\n");
    }
    if (vr == QCA::TLS::Valid)
        printf("Valid certificate.\n");
    else
        printf("Invalid certificate: %s\n", resultToString(vr).latin1());

    printf("Let's try a GET request now.\n");
    QString req = "GET / HTTP/1.0\nHost: " + host + "\n\n";
    QCString cs = req.latin1();
    QByteArray buf(cs.length());
    memcpy(buf.data(), cs.data(), buf.size());
    ssl->write(buf);
}

void TTCP::ssl_readyRead()
{
    QByteArray a = ssl->read();
    QCString cs;
    cs.resize(a.size()+1);
    memcpy(cs.data(), a.data(), a.size());
    printf("%s", cs.data());
}

void TTCP::ssl_readyReadOutgoing(int)
{
    QByteArray a = ssl->readOutgoing();
    socket->writeBlock(a.data(), a.size());
}

void TTCP::ssl_closed()
{
    printf("SSL session closed\n");
}

void TTCP::ssl_error(int x)
{
    if (x == QCA::TLS::ErrHandshake) {
        printf("SSL Handshake Error!\n");
//        quit();
    }
    else {
        printf("SSL Error!\n");
//       quit();
    }
    delete ssl;
    ssl = NULL;
}


void TTCP::showCertInfo(const QCA::Cert &cert)
{
    printf("-- Cert --\n");
    printf(" CN: %s\n", cert.subject()["CN"].latin1());
    printf(" Valid from: %s, until %s\n",
        cert.notBefore().toString().latin1(),
        cert.notAfter().toString().latin1());
    printf(" PEM:\n%s\n", cert.toPEM().latin1());
}


QString TTCP::resultToString(int result)
{
    QString s;
    switch(result) {
        case QCA::TLS::NoCert:
            s = QObject::tr("No certificate presented.");
            break;
        case QCA::TLS::Valid:
            break;
        case QCA::TLS::HostMismatch:
            s = QObject::tr("Hostname mismatch.");
            break;
        case QCA::TLS::Rejected:
            s = QObject::tr("Root CA rejects the specified purpose.");
            break;
        case QCA::TLS::Untrusted:
            s = QObject::tr("Not trusted for the specified purpose.");
            break;
        case QCA::TLS::SignatureFailed:
            s = QObject::tr("Invalid signature.");
            break;
        case QCA::TLS::InvalidCA:
            s = QObject::tr("Invalid CA certificate.");
            break;
        case QCA::TLS::InvalidPurpose:
            s = QObject::tr("Invalid certificate purpose.");
            break;
        case QCA::TLS::SelfSigned:
            s = QObject::tr("Certificate is self-signed.");
            break;
        case QCA::TLS::Revoked:
            s = QObject::tr("Certificate has been revoked.");
            break;
        case QCA::TLS::PathLengthExceeded:
            s = QObject::tr("Maximum cert chain length exceeded.");
            break;
        case QCA::TLS::Expired:
            s = QObject::tr("Certificate has expired.");
            break;
        case QCA::TLS::Unknown:
        default:
            s = QObject::tr("General validation error.");
            break;
    }
    return s;
}

QCA::Cert TTCP::readCertXml(const QDomElement &e)
{
    QCA::Cert cert;
    // there should be one child data tag
    QDomElement data = e.elementsByTagName("data").item(0).toElement();
    if (!data.isNull())
	cert.fromDER(Base64::stringToArray(data.text()));
    return cert;
}

QPtrList<QCA::Cert> TTCP::getRootCerts(const QString &store)
{
    QPtrList<QCA::Cert> list;

    // open the Psi rootcerts file
    QFile f(store);
    if (!f.open(IO_ReadOnly)) {
        printf("unable to open %s\n", f.name().latin1());
        return list;
    }
    QDomDocument doc;
    doc.setContent(&f);
    f.close();

    QDomElement base = doc.documentElement();
    if (base.tagName() != "store") {
        printf("wrong format of %s\n", f.name().latin1());
        return list;
    }
    QDomNodeList cl = base.elementsByTagName("certificate");
    if (cl.count() == 0) {
        printf("no certs found in %s\n", f.name().latin1());
        return list;
    }

    int num = 0;
    for (int n = 0; n < (int)cl.count(); ++n) {
        QCA::Cert *cert = new QCA::Cert(readCertXml(cl.item(n).toElement()));
        if(cert->isNull()) {
            printf("error reading cert\n");
            delete cert;
            continue;
        }

        ++num;
        list.append(cert);
    }
    printf("imported %d root certs\n", num);

    return list;
}

#endif

/* eof */
