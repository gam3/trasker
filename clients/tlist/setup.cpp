
#include <iostream>

#include <QCheckBox>
#include "setup.h"

Setup::Setup(QWidget * parent):QDialog (parent)
{
    setupUi(this);
}

Setup::Setup(const QString & host, const quint16 & port, const bool & ssl,
	     const QString & user, const QString & password,
	     QWidget * parent) : QDialog (parent)
{
    setupUi(this);
    hostEdit->setText(host);
    portEdit->setText(QString("%1").arg(port));
    userEdit->setText(user);
    if (password.size()) {
	passwordEdit->setText(password);
	remPass->setCheckState(Qt::Checked);
    }
    if (ssl)
	useSSL->setCheckState(Qt::Checked);
}

#include <QSettings>

void Setup::saveData()
{
    QString host = hostEdit->text();
    QString user = userEdit->text();
    QString password = passwordEdit->text();
    qint16 port = portEdit->text().toInt();
    bool ssl = useSSL->isChecked();

    QSettings settings("Tasker", "tlist");

    settings.beginGroup("Host");
    settings.setValue("host", host);
    settings.setValue("port", port);
    settings.setValue("ssl", ssl);
    settings.endGroup();

    settings.beginGroup("User");
    settings.setValue("user", user);
    if (remPass->isChecked())
	settings.setValue("password", password);
    settings.endGroup();

    portEdit->setText(QString("%1").arg(port));
    userEdit->setText(user);
    passwordEdit->setText(password);

    emit setup(host, port, ssl, user, password);
    hide();
}

/* eof */
