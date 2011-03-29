
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
	qWarning("connect");
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
}

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

void TestQString::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

// QTEST_MAIN(TestQString)
QTEST_MAIN(TestTTCP)

#include "test.moc"
