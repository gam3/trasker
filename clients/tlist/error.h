/*
 *  Copyright 2009 (C) G. Allen Morris III
 */
#ifndef ERROR_H
#define ERROR_H

/*! \file error.h
 *
 * This is a generalized window to display error messages.
 *
 * \brief The general error window
 *
 */

/*! Image of the error window widget
 * \image html errorwindow.png "The add note widget"
 */

#include <QtGui>
#include "ui_error.h"

class ErrorWindow : public QDialog, private Ui::ErrorWindow {
    Q_OBJECT
public:
    inline ErrorWindow(QWidget *parent) : QDialog(parent) { setupUi(this); }
    inline void setText(const QString &error)
    {
        errorText->setPlainText(error);
    }
};

#endif
