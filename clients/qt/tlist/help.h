

#ifndef HELP_H
#define HELP_H

/*! image of the note widget
 * \image html notes.png "The add note widget"
 */

#include <QWebView>
#include "ui_help.h"

class TTCP;

class HelpWin : public QDialog, private Ui::HelpWin {
    Q_OBJECT
public:
    HelpWin(QWidget *parent = 0);
};

#endif
