/****************************************************************************
**
** Copyright (C) 2011 G. Allen Morris III
** Contact: <gam3@gam3.net>
**
**
****************************************************************************/

/*
    timeeditdelegate.cpp

    A delegate that allows the user to edit some aspects of a timeslice
*/

#include <QtGui>

#include "timeeditdelegate.h"

TimeEditDelegate::TimeEditDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    timeExp.setPattern("([0-9]{,2}):([0-9]{,2}):([0-9]{,2})");
}

void TimeEditDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if (index.column() == 0) {
        QString x = index.data().toString();

        int progress = 30;
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;
        progressBarOption.text = QString::number(progress) + "%";
        progressBarOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);
    } else {
	QItemDelegate::paint(painter, option, index);
    }
}

QWidget *TimeEditDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    Q_UNUSED(option);

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
#if 0
    if (!isSupportedType(originalValue.type()))
	return 0;
#endif
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(false);

    QRegExp regExp;

    switch(index.column()) {
       case 1:
         regExp = timeExp;
	 break;
       case 4:
         break;
       default:
         qWarning("%d %d", index.column(), 1);
	 return 0;
    }
//    return QItemDelegate::createEditor(parent, option, index);
    if (!regExp.isEmpty()) {
        QValidator *validator = new QRegExpValidator(regExp, lineEdit);
        lineEdit->setValidator(validator);
    }

    return lineEdit;
}

void TimeEditDelegate::setEditorData(QWidget *,
                                    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::EditRole);
    qWarning("%s", qPrintable(value.toString()));
}

void TimeEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QString x = ((QLineEdit *)editor)->text();
    QVariant value(x);;
    model->setData(index, value);
}


#if 0
void TimeEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
qWarning("abob");
//    editor->setGeometry(option.rect);
}
#endif

