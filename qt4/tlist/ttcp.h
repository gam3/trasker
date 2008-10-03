
#ifndef TTCP_H
#define TTCP_H

#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QTime>

#include "connection.h"

class TTCP : public QObject
{
    Q_OBJECT

public:
    TTCP(const char *, int, bool);
    ~TTCP();

    void sendMessage(const QString &message);
    QString nickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;

signals:
    void add_entry(QString,int,int,QTime,QTime);
    void current(int project_id);
    void disable(int project_id);
    void settime(int project_id, QTime time, QTime atime);
    void project(QString name, QString action);
    void accept_project(QString name);
    void accept_note(QString name);
    void accept_select(QString name);
    void error(QString error_string);
    void update_all();

protected slots:
    void setConnected();
    void setunConnected();

private slots:
    void newConnection(Connection *connection);
    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();
    void newCommand(const QStringList &list);

private:
    void removeConnection(Connection *connection);

    Connection *connection;
};

#endif


