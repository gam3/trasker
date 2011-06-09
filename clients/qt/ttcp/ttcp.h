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

#include <QHostAddress>
#include <QTime>

#include "connection.h"

/** This is Tasker Server object
 *
 * \brief The Taster Server object cotrols all the commands and
 * responses to the Trasker Server.
 *
 * The 
 */

class TTCP : public QObject
{
    Q_OBJECT
public:
    TTCP(const QString &host, quint16 port, bool ssl, const QString &user, const QString &password);
    TTCP(Connection &connection);
    ~TTCP();

public slots:
    /*! \brief Request the current elapsed time
     
     \since 0.01
     \see add_entry
     */
    void gettime(int projId);
    //! \since 0.01
    void getauto(int projId);
    /*! Add a note to a project

     Add a note for a project or if project equal to zero add a General note.
     If the note was added a accept_note() signal will be sent.
    \since 0.01
    \see accept_note
     */
    void addnote(int projId, const QString &note) const;
    //! \since 0.01
    void addtask(int parentId, const QString &name, const QString &desc) const;
    //! \since 0.01
    void addauto(const int id, const QString &host, const QString &classN, const QString &name,
                 const QString &role, const QString &title, const QString &desktop) const;
    //! Change the current project
    //! \since 0.01
    void setProject(int);

    //! \since 0.01
    void getTimes(void);
    //! \since 0.01
    void getTimes(QDate);
    //! \since 0.01
    void getTimes(QDate, QDate);

    //! \since 0.01
    void getRecentProjects();

    //! \since 0.01
    void timesliceChangeProject(int id, int, int);
    //! \since 0.01
    void timesliceChangeTime(int id, const QDateTime &, const QDateTime &);
    //! \since 0.01
    void timesliceSplitTime(const int id, const QDateTime &time);
    //! \since 0.01
    void timesliceSplitTime(const int id);

signals:
    /*! TTCP is connected and authorized

     After the connected signal has been emited you can send messages to the
     Trasker Server and recieve messages from it until a disconnect() signal 
     emitted.
     */
    void connected();
    /*! TTCP is disconnect
     After this signal is received no more messages will be sent to the Trasker
     Server
     */
    void disconnected();

    //! The note was added \see addnote \since 0.01
    void accept_note(QString name);
    //! The project was added \see addtask \since 0.01
    void accept_project(QString name);
    //! The auto select filter was added \see addauto \since 0.01
    void accept_select(QString name);
    /*! \brief A project time entry for the current day
     \see gettime
     \since 0.01
     */
    void add_entry(QString user, int project_id, int parent_project_id, QTime time, QTime atime);
    void add_timeslice(QString user, int timeclice_id, int project_id, int auto_id, QString from, QDateTime startTime, QString duration);
    void alert_end_message(int alert_id);
    void alert_message(int alert_id, QString title, QString description);
    void add_autoentry(QString user, int project_id, int auto_id, QString, QString, QString, QString, QString, QString, QString, QString);

    void current(int project_id);
    void disable(int project_id);
    void error(QString error_string);
    void project(QString name, QString action);
    void settime(int project_id, QTime time, QTime atime);
    void update_all();

    /*! \brief recentproject

     \see getRecentProjects
     */

    void recentproject(int project_id, int index, int max, QString uniqueName);
    /*! \brief recentprojects

     \see getRecentProjects
     */
    void recentprojects(QList<int>&);

    /*! \brief The hourly signal is emited once per hour.

     \since 0.01
     */
    void hourly();
    /*! \brief The daily signal is emited when a new day is starting.
    
     This is dependent on the servers time zone and may not happen and midnight.
     \since 0.01
     */
    void daily();


public:
    void sendMessage(const QString &message);
    QString nickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    QString getProject(int) const;
    //* returns true if servery connect is ready for use \since 0.01
    bool isReadyForUse(void) const;

protected slots:
    void setConnected();
    void setunConnected();

private slots:
    void newConnection(Connection *connection);
    void readyForUse();
    void notReadyForUse();
    void newCommand(const QStringList &list);
    void setAuto(QString &host, QString &classN, QString &name, QString &role, QString &title, QString &desktop);

private:
    void removeConnection(Connection *connection);

    bool readyForUseFlag;

    const QString &user;
    const QString &password;
    const QString &host;
    quint16 port;
    bool    ssl;

    Connection *connection;
};

#endif
