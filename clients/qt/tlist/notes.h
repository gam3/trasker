
#ifndef NOTES_H
#define NOTES_H

/*! image of the note widget
 * \image html notes.png "The add note widget"
 */

#include "ui_notes.h"

//#include <iostream>

class TTCP;
class Project;

/*! \brief The "add a note" widget.
 *
 */

class Notes : public QDialog, private Ui::Notes {
    Q_OBJECT
public:
    Notes(const TTCP *, QWidget *parent = 0);
    void setProject(const Project &proj);

public slots:
    void notesDone(QString Check);

private slots:
    void add_note();
    void show_help();

private:
    const TTCP *ttcp;
    const Project *project;
};

#endif
