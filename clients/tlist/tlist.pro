# #####################################################################
# #####################################################################
# MAKEFILE = qmakefile
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt \
    debug
QT += network \
    webkit

VERSION = 0.0.1
DEFINES += HAVE_CONFIG_H

# Input
HEADERS += addproject.h \
    alerts.h \
    auto_select.h \
    conf.h \
    config.h \
    connection.h \
    error.h \
    help.h \
    mainwindow.h \
    mytreeview.h \
    notes.h \
    project.h \
    projects.h \
    setup.h \
    treeitem.h \
    treemodel.h \
    ttcp.h \
    timeedit.h \
    timemodel.h \
    timeitem.h \
    cmdline.h

FORMS += addproject.ui \
    auto_select.ui \
    error.ui \
    help.ui \
    mainwindow.ui \
    notes.ui \
    projects.ui \
    setup.ui \
    timeedit.ui \
    alerts.ui

SOURCES += addproject.cpp \
    auto_select.cpp \
    alerts.cpp \
    connection.cpp \
    error.cpp \
    help.cpp \
    main.cpp \
    mainwindow.cpp \
    mytreeview.cpp \
    notes.cpp \
    projects.cpp \
    setup.cpp \
    treeitem.cpp \
    treemodel.cpp \
    ttcp.cpp \
    timeedit.cpp \
    timemodel.cpp \
    timeitem.cpp \
    cmdline.c

DISTFILES += tlist.ggo

cmdline_c.target = cmdline.c
cmdline_c.commands = gengetopt < tlist.ggo
cmdline_c.depends = cmdline_h

cmdline_h.target = tlist.ggo

QMAKE_EXTRA_TARGETS += cmdline_c cmdline_h

MOC_DIR += .moc
RESOURCES += systray.qrc \
    tlist.qrc
unix:SOURCES += x11.cpp

RC_FILE = pics/tlist.icns
