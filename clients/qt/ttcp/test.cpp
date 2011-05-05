
#include <iostream>
using namespace std;

#include "ttcp.h"

#include <QtTest/QTest>
#include <QObject>

class TestConnection : public Connection
{
    Q_OBJECT
public:
    TestConnection(QObject *parent) : Connection(parent) {
	Q_UNUSED(parent);
    };
};

class TestTTCP: public QObject
{
    Q_OBJECT
private slots:
    void connect();
};

void TestTTCP::connect()
{
    TestConnection *connection = new TestConnection(this);
    qWarning() << connection;
    connection->setAuthorize("asdfsdf", "bob");
    qWarning() << connection->name();
    connection->setAuthorize("bob", "bob");
    qWarning() << connection->name();
    connection->setHost("127.0.0.1", (qint16)8000, false);
}

QTEST_MAIN(TestTTCP)

#include "test.moc"
