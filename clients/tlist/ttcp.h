/****************************************************************************
**
** Copyright (C) G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*! \file ttcp.h
 *  \brief The QT interface to the Tasker Server
 *
 *  This is the QT interface to the Taster Server....
 *  It connects to the server with or without ssl.
 */

#ifndef TTCP_H
#define TTCP_H

#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QTime>

#include "connection.h"

/** This is Tasker Server object
 *
 */

class TTCP : public QObject
{
    Q_OBJECT
public:
    TTCP(const QString &host, quint16 port, bool ssl, const QString &user, const QString &password);
    ~TTCP();

public slots:
    //! Request the current elapsed time
    void gettime(int projId);
    void getauto(int projId);
    void addnote(int projId, const QString &note) const;
    //! Change the current project
    void setProject(int);

signals:
    void accept_note(QString name);
    void accept_project(QString name);
    void accept_select(QString name);
    void add_entry(QString user, int project_id, int parent_project_id, QTime time, QTime atime);
    void current(int project_id);
    void disable(int project_id);
    void disconnected();
    void connected();
    void error(QString error_string);
    void project(QString name, QString action);
    void settime(int project_id, QTime time, QTime atime);
    void update_all();

public:
    void sendMessage(const QString &message);
    QString nickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;

protected slots:
    void setConnected();
    void setunConnected();

private slots:
    void newConnection(Connection *connection);
    void readyForUse();
    void newCommand(const QStringList &list);
    void setAuto(QString &host, QString &classN, QString &name, QString &role, QString &title, QString &desktop);

private:
    void removeConnection(Connection *connection);

    QString *name, *password;
    QString const &user;
    quint16 port;
    bool ssl;

    Connection *connection;
};

#endif


