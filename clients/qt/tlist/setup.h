
#ifndef SETUP_H
#define SETUP_H

#include <QDialog>

#include "ui_setup.h"

class Setup : public QDialog, private Ui::SetupHost {
    Q_OBJECT

public:
    Setup(QWidget *parent = 0);
    Setup(const QString & host, const quint16 &port, const bool &ssl, const QString & user, const QString & password, QWidget * parent = 0);

private slots:
    void on_nonStandard_clicked(bool checked);
    void on_useSSL_clicked(bool checked);
    void on_done_clicked();
    void saveData();

signals:
    void setup(QString host, quint16 port, bool ssl, QString user, QString password);
};

#endif
