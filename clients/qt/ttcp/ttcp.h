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
    TTCP(Connection &connection);
    ~TTCP();

public slots:
    //! Request the current elapsed time
    void gettime(int projId);
    void getauto(int projId);
    void addnote(int projId, const QString &note) const;
    void addtask(int parentId, const QString &name, const QString &desc) const;
    void addauto(const int id, const QString &host, const QString &classN, const QString &name,
                 const QString &role, const QString &title, const QString &desktop) const;
    //! Change the current project
    void setProject(int);

    void getTimes(void);
    void getTimes(QDate);
    void getTimes(QDate, QDate);

    void getRecentProjects();

signals:
    void accept_note(QString name);
    void accept_project(QString name);
    void accept_select(QString name);
    void add_autoentry(QString user, int project_id, int auto_id, QString, QString, QString, QString, QString, QString, QString, QString);
    void add_entry(QString user, int project_id, int parent_project_id, QTime time, QTime atime);
    void add_timeslice(QString user, int timeclice_id, int project_id, int auto_id, QString from, QDateTime startTime, QString duration);
    void alert_end_message(int alert_id);
    void alert_message(int alert_id, QString title, QString description);
    void connected();
    void current(int project_id);
    void disable(int project_id);
    void disconnected();
    void error(QString error_string);
    void project(QString name, QString action);
    void settime(int project_id, QTime time, QTime atime);
    void update_all();
    void hourly();
    void recentproject(int project_id, int index, int max, QString uniqueName);
    void recentprojects(QList<int>&);

public:
    void sendMessage(const QString &message);
    QString nickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    QString getProject(int) const;

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

    const QString &user;
    const QString &password;
    const QString &host;
    quint16 port;
    bool    ssl;

    Connection *connection;
};

#endif


