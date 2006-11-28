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

#include <qstring.h>
#include <qsocket.h>
#include <qtimer.h>
#include <qnetworkprotocol.h>

class TTCP : public QNetworkProtocol
{
    Q_OBJECT

public:
    TTCP();
    TTCP(const char *host, int port);
    virtual ~TTCP();
    virtual int supportedOperations() const;
    void get_entries(QString);
    void needtime(QString, int);
    void start(QString, int);
    void projectadd( QString user, const int parent, QString name, QString description );
    void noteadd( const char * user, const int project, const char *name );
    QTimer *timer;

protected:
    virtual void operationListChildren( QNetworkOperation *op );
    virtual void operationGet( QNetworkOperation *op );

    QSocket *commandSocket;
    bool connectionReady;
    bool readGroups;
    bool readArticle;

private:
    bool checkConnection( QNetworkOperation *op );
    void close();
    void parseGroups();
    void parseArticle();
    const char *host;
    int  port;
    float version;

signals:
    void entry(int user_id, QString name, int project_id, int parent_id, QTime time, QTime atime);
    void current(int project_id);
    void disable(int project_id);
    void settime(int project_id, QTime time, QTime atime);
    void project(QString name, QString action);
    void accept_project(QString name);
    void accept_note(QString name);
    void error(QString error_string);
    void update_all();
    void connected();
    void disconnected();

protected slots:
    void hostFound();
    void myConnected();
    void closed();
    void readyRead();
    void error( int );
    void timeout();
};

#endif

