
#ifndef ADDPROJECT_H
#define ADDPROJECT_H

/*! image of the Auto Select widget
 *
 * \image html auto_select.png "The add note widget"
 */

#include "ui_addproject.h"

class TTCP;
class Project;

class AddProject : public QDialog, private Ui::AddProject {
    Q_OBJECT
public:
    AddProject(const TTCP *ttcp, QWidget *parent = 0);
    void setParentProject(const Project &proj);

private slots:
    void add_project();
    void show_help();
private:
    const TTCP *ttcp;
    const Project *parentProject;
};

#endif
