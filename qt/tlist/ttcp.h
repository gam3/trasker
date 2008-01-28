/****************************************************************************
** $Id: $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TTCP_H
#define TTCP_H

#include "../conf.h"
#include <qstring.h>
#include <qsocket.h>
#include <qtimer.h>
#include <qptrlist.h>
#include <qnetworkprotocol.h>
#include <qdom.h>

#if HAVE_QCA_H
# include <qca.h>
#endif

class TTCP : public QNetworkProtocol
{
    Q_OBJECT

public:
//    TTCP();
    TTCP(const char *host, int port, bool);
//    TTCP(const QString&, int);
    virtual ~TTCP();
    virtual int supportedOperations() const;
    void start(const char *, int);
    void get_entries(QString);
    void needtime(QString, int);
    void start(QString, int);
    void projectadd( QString user, const int parent, QString name, QString description );
    void noteadd( const char * user, const int project, const char *name );
    QTimer *timer;
    void setauto( const char * user, const int project_id,
                  QString host, QString name, QString Class,
		  QString title, QString role, QString desktop);

protected:
    virtual void operationListChildren( QNetworkOperation *op );
    virtual void operationGet( QNetworkOperation *op );

    QSocket   *socket;
#if HAVE_QCA_H
    QCA::TLS  *ssl;
    QCA::Cert  cert;
    QPtrList<QCA::Cert> rootCerts;
#endif

    bool connectionReady;
    bool readGroups;
    bool readArticle;

private:
    bool checkConnection( QNetworkOperation *op );
    void close();
    void parseGroups();
    void parseArticle();
    QString host;
    Q_UINT16 port;
    float version;

    QString resultToString(int result);

#if HAVE_QCA_H
    void showCertInfo(const QCA::Cert &cert);
    QPtrList<QCA::Cert> getRootCerts(const QString &store);
    QCA::Cert readCertXml(const QDomElement &e);
#endif

    void doTheRead(QSocket *socket);


signals:
    void entry(int user_id, QString name, int project_id, int parent_id, QTime time, QTime atime);
    void current(int project_id);
    void disable(int project_id);
    void settime(int project_id, QTime time, QTime atime);
    void project(QString name, QString action);
    void accept_project(QString name);
    void accept_note(QString name);
    void accept_select(QString name);
    void error(QString error_string);
    void update_all();
    void connected();
    void disconnected();

protected slots:
    void sock_hostFound();
    void sock_connected();
    void sock_readyRead();
    void sock_closed();
    void sock_error( int );
    void timeout();

#if HAVE_QCA_H
    void ssl_handshaken();
    void ssl_readyRead();
    void ssl_closed();
    void ssl_error(int);
    void ssl_readyReadOutgoing(int);
#endif

};

#endif

