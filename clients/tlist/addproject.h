
#ifndef ADDPROJECT_H
#define ADDPROJECT_H

/*! image of the Auto Select widget
 *
 * \image html auto_select.png "The add note widget"
 */

#include "ui_addproject.h"

class TTCP;

class AddProject : public QDialog, private Ui::AddProject {
    Q_OBJECT
public:
    AddProject(QWidget *parent = 0);
    void setProjectName(const QString &name);

private slots:
    void add_project();
    void show_help();
private:
    TTCP *ttcp;
};

#endif
