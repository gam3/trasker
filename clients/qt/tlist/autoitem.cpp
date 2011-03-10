/****************************************************************************
**
** Copyright (C) 2006-2011 G. Allen Morris III, All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QString>
#include <QStringList>
#include "autoitem.h"

AutoItem::AutoItem (AutoItem * parent)
{
    Q_UNUSED(parent);
}

AutoItem::AutoItem(QString user, int autoselect_id, int project_id, QString host,
                   QString name, QString classname, QString role, QString desktop,
		   QString title, QString flags)
		   : autoselect_id(autoselect_id), project_id(project_id), userd(user),
		     hostd(host), named(name), classnamed(classname), roled(role),
		     desktopd(desktop), titled(title), flagsd(flags)

{
    qWarning() << "AutoItem: " << autoselect_id << " host(" << host << ") title(" << title << ") flags(" << flags << ") name("
	       << name << ") class(" << classname << ") role(" << role << ") desktop(" << desktop << ")";
}

AutoItem::~AutoItem ()
{
}

QString AutoItem::id()
{
    return QString("%1").arg(autoselect_id);
}

QString AutoItem::project()
{
    return QString("%1").arg(project_id);
}

QString AutoItem::user()
{
    return QString("%1").arg(userd);
}

QString AutoItem::host()
{
    return hostd;
}
QString AutoItem::name()
{
    return named;
}
QString AutoItem::classname()
{
    return classnamed;
}
QString AutoItem::role()
{
    return roled;
}
QString AutoItem::desktop()
{
    return desktopd;
}
QString AutoItem::title()
{
    return titled;
}
QString AutoItem::flags()
{
    return flagsd;
}

