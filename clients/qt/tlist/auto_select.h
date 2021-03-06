
#ifndef AUTO_SELECT_H
#define AUTO_SELECT_H

/*! image of the Auto Select widget
 *
 * \image html auto_select.png "The add note widget"
 */

#include "ui_auto_select.h"

class QProcess;
class TTCP;
class Project;

class AddAuto : public QMainWindow, private Ui::AddAuto {
    Q_OBJECT
public:
    AddAuto(TTCP *, QWidget *parent = 0);
    void setProject(const Project &proj);
    void setProjectName(const QString &name);

private slots:
    void buttonHelp_clicked();
    void grabClicked();
    void setAuto();

    void read();

public slots:
    void autoDone(QString Check);
    void update_display(const QModelIndex &index);

private:
    QProcess *process;
    TTCP *ttcp;
    const Project *project;
};

#endif
