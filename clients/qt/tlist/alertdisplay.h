#ifndef ALERTDISPLAY_H
#define ALERTDISPLAY_H

#include <QtGui/QWidget>

namespace Ui {
    class AlertDisplay;
}

class AlertDisplay : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(AlertDisplay)
public:
    explicit AlertDisplay(QWidget *parent = 0);
    virtual ~AlertDisplay();

    void setText(const QString);

public slots:
    void reject() { };
    void accept() { };

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::AlertDisplay *m_ui;
};

#endif // ALERTDISPLAY_H
