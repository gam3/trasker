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

TimeEditDelegate::TimeEditDelegate(QObject *parent, TimeModel *model)
    : QItemDelegate(parent), model(model)
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

    QVariant originalValue = index.model()->data(index, Qt::EditRole);

    switch(index.column()) {
      case 1:
        {
	    QTimeEdit *timeEdit = new QTimeEdit(parent);
	    timeEdit->setFrame(false);
	    timeEdit->setFocusPolicy(Qt::StrongFocus);

	    QDateTime start = index.model()->data(index.sibling(index.row() - 1, index.column()), Qt::EditRole).toDateTime();
	    QDateTime end = index.model()->data(index.sibling(index.row(), c_endtime), Qt::EditRole).toDateTime();
	    QDateTime current = originalValue.toDateTime();
	    if (!start.isValid()) {
		return NULL;
	    }
    qWarning("to %d", originalValue.type());
    qWarning("o %s", qPrintable(current.toString()));
    qWarning("p %s", qPrintable(start.toString()));
    qWarning("d %s", qPrintable(end.toString()));
            timeEdit->setDateTime(current);
            timeEdit->setDateTimeRange(start, end);
	    return timeEdit;
	}
      case c_project:
        {
	    QLineEdit *lineEdit = new QLineEdit(parent);
	    lineEdit->setFrame(false);
/*
	    QCompleter *completer = new QCompleter(model->getProjectList(), lineEdit);
	    completer->setCaseSensitivity(Qt::CaseInsensitive);
	    lineEdit->setCompleter(completer);
 */
	    return lineEdit;
	}
	break;
      default:
	qWarning("%d %d", index.column(), 1);
    }

    return NULL;
}

void TimeEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::EditRole);

//    qWarning("setEditor %s", qPrintable(value.toString()));

    switch(index.column()) {
      case c_datetime:
        break;
      case c_project:
        {
	  QCompleter *completer = new QCompleter(model->getProjectList(), (QLineEdit *)editor);
	  completer->setCaseSensitivity(Qt::CaseInsensitive);
	  ((QLineEdit *)editor)->setCompleter(completer);
	  ((QLineEdit *)editor)->setText(value.toString());
	}
      default:
        break;
    }
}

void TimeEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    qWarning("setModelData");
    switch(index.column()) {
      case 4:
        {
	    QString x = ((QLineEdit *)editor)->text();
	    QVariant value(x);;
	    model->setData(index, value);
	}
    }
    qWarning("x setModelData");
}


#if 0
void TimeEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
qWarning("abob");
//    editor->setGeometry(option.rect);
}
#endif

