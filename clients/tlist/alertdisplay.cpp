#include "alertdisplay.h"
#include "ui_alertdisplay.h"

AlertDisplay::AlertDisplay(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AlertDisplay)
{
    m_ui->setupUi(this);
}

AlertDisplay::~AlertDisplay()
{
    delete m_ui;
}

void AlertDisplay::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AlertDisplay::setText(const QString description)
{
    m_ui->textBrowser->append(description);
}
