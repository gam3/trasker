/****************************************************************************
**
** Copyright (C) 2006-2011 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef AUTOITEM_H
#define AUTOITEM_H

#include <QtDebug>
#include <QString>
#include <QList>
#include <QVariant>
#include <QVector>

class AutoItem
{
public:
    AutoItem(AutoItem *parent = 0);
    AutoItem(QString, int, int, QString, QString, QString, QString, QString, QString, QString);

    ~AutoItem();

    QString id(void);
    QString project(void);
    QString user(void);
    QString host(void);
    QString name(void);
    QString classname(void);
    QString role(void);
    QString desktop(void);
    QString title(void);
    QString flags(void);

private:
    int autoselect_id;
    int project_id;
    QString userd;
    QString hostd;
    QString named;
    QString classnamed;
    QString roled;
    QString desktopd;
    QString titled;
    QString flagsd;
};

#endif
