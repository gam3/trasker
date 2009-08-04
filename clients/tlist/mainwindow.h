
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "ui_mainwindow.h"

class TTCP;


class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(TTCP *, QWidget *parent = 0);
    ~MainWindow();

private:
    TTCP *ttcp;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif
