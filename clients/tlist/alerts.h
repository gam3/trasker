
#ifndef ADDPROJECT_H
#define ADDPROJECT_H

/*! image of the Auto Select widget
 *
 * \image asdf
 */

#include "ui_alerts.h"

class TTCP;
class Project;

class Alerts : public QDialog, private Ui::Alerts {
    Q_OBJECT
public:
    Alerts(const TTCP *ttcp, QWidget *parent = 0);
    void setParentProject(const Project &proj);
    QTimer *timer;
public slots:
    void add(QString text);
    void update();
private slots:
    void show_help();
private:
    const TTCP *ttcp;
    const Project *parentProject;
};

#endif