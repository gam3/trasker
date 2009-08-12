
#include <iostream>

#include <QCheckBox>
#include <QSettings>
#include "setup.h"

Setup::Setup(QWidget * parent):QDialog (parent)
{
    setupUi(this);

    QSettings settings("Trasker", "tlist");

    settings.beginGroup("Host");
    hostEdit->setText(settings.value("host").toString());
    portEdit->setText(settings.value("port").toString());
    bool x = settings.value("ssl", 1).toBool();
    if (x) {
	useSSL->setCheckState(Qt::Checked);
    } else {
	useSSL->setCheckState(Qt::Unchecked);
    }
    settings.endGroup();

    settings.beginGroup("User");
    loginEdit->setText(settings.value("login").toString());
    userEdit->setText(settings.value("user").toString());
    passwordEdit->setText(settings.value("password").toString());
    settings.endGroup();
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


void Setup::saveData()
{
    QString host = hostEdit->text();
    QString user = userEdit->text();
    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    qint16 port = portEdit->text().toInt();
    bool ssl = useSSL->isChecked();

    QSettings settings("Trasker", "tlist");

    settings.beginGroup("Host");
    settings.setValue("host", host);
    settings.setValue("port", port);
    settings.setValue("ssl", ssl);
    settings.endGroup();

    settings.beginGroup("User");
    settings.setValue("login", login);
    settings.setValue("user", user);
    if (remPass->isChecked())
	settings.setValue("password", password);
    else
        settings.setValue("password","");
    settings.endGroup();

    portEdit->setText(QString("%1").arg(port));
    userEdit->setText(user);
    passwordEdit->setText(password);

    emit setup(host, port, ssl, user, password);

    hide();
}

void Setup::on_done_clicked()
{
    QString host = hostEdit->text();
    QString user = userEdit->text();
    QString password = passwordEdit->text();
    qint16 port = portEdit->text().toInt();
    bool ssl = useSSL->isChecked();

    saveData();

    emit setup(host, port, ssl, user, password);
    close();
}

void Setup::on_useSSL_clicked(bool checked)
{
    portEdit->setText(checked ? "8001" : "8000");
}

void Setup::on_nonStandard_clicked(bool checked)
{
    label_4->setEnabled(checked);
    portEdit->setEnabled(checked);
}
