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
    if (index.column() == 99) {
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
      case c_datetime:
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
        if (1) {
	    QComboBox *editor = new QComboBox(parent);
	    QMap<QString, int> list = model->getProjectList();
	    QMapIterator<QString, int> i(list);
	    while (i.hasNext()) {
		i.next();
		editor->addItem(i.key(), QVariant(i.value()));
	    }
	    return editor;
	} else {
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

/*
 This is called when the editor is done.
 */

void TimeEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);

//    qWarning("setEditor %s", qPrintable(value.toString()));

    switch(index.column()) {
      case c_datetime:
        break;
      case c_project:
        if (1) {
	    QString str = value.toString();
	    QComboBox *comboBox = static_cast<QComboBox*>(editor);
	    comboBox->setCurrentIndex( comboBox->findData(value) );
	} else {
	    QCompleter *completer = new QCompleter(model->getProjectList().uniqueKeys(), (QLineEdit *)editor);
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
    int timesliceid = 0;
    timesliceid = index.data(Qt::UserRole).toInt();
    switch(index.column()) {
      case c_datetime:
        {
          QVariant originalValue = index.data(Qt::EditRole);
	  const QDateTime current = originalValue.toDateTime();
	  QTimeEdit *edit = static_cast<QTimeEdit*>(editor);
	  const QDateTime new_time = edit->dateTime();
// qWarning() << new_time.toString(Qt::TextDate) << current.toString(Qt::TextDate);
	  emit timesliceChangeTime( timesliceid, new_time, current);
	  break;
	}
      case 2:
qWarning() << "setModelData " << index.column();
        break;
      case c_project:
        {
	    int new_prjid = 0;
	    int old_prjid = 0;

	    timesliceid = index.data(Qt::UserRole).toInt();
	    old_prjid = index.data(Qt::EditRole).toInt();

	    if (1) {
		QComboBox *comboBox = static_cast<QComboBox*>(editor);
		new_prjid = comboBox->itemData(comboBox->currentIndex()).toInt();
	    } else {
		QString x = ((QLineEdit *)editor)->text();
		QVariant value(x);
		model->setData(index, value);
	    }
	    emit timesliceChangeProject( timesliceid, new_prjid, old_prjid);
	}
	break;
      default:
qWarning() << "setModelData " << index.column();
	break;
    }
}

#if 0
void TimeEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
qWarning("abob");
    editor->setGeometry(option.rect);
}
#endif

