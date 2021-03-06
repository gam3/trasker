# #####################################################################
# #####################################################################
# MAKEFILE = qmakefile
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt
CONFIG += debug
QT += network \
    webkit
VERSION = 0.0.1
DEFINES += HAVE_CONFIG_H
DESTDIR = .

# target.path = /usr/local/bin
INSTALLS += target

LIBS += -L/usr/lib -L ../ttcp -lttcp

INCLUDEPATH = ../ttcp

# Input
HEADERS += addproject.h \
    alerts.h \
    auto_select.h \
    automodel.h \
    autoitem.h \
    conf.h \
    config.h \
    error.h \
    help.h \
    mainwindow.h \
    mytreeview.h \
    mytableview.h \
    notes.h \
    project.h \
    projects.h \
    setup.h \
    treeitem.h \
    treemodel.h \
    timeedit.h \
    timeeditdelegate.h \
    timemodel.h \
    timeitem.h \
    multipagewidget.h \
    cmdline.h \
    alertdisplay.h \
    projectlist.h

FORMS += addproject.ui \
    auto_select.ui \
    error.ui \
    help.ui \
    mainwindow.ui \
    notes.ui \
    projects.ui \
    setup.ui \
    timeedit.ui \
    alerts.ui \
    alert_entry.ui \
    alertdisplay.ui
SOURCES += addproject.cpp \
    auto_select.cpp \
    automodel.cpp \
    autoitem.cpp \
    alerts.cpp \
    error.cpp \
    help.cpp \
    main.cpp \
    mainwindow.cpp \
    mytreeview.cpp \
    mytableview.cpp \
    notes.cpp \
    projects.cpp \
    setup.cpp \
    treeitem.cpp \
    treemodel.cpp \
    timeedit.cpp \
    timeeditdelegate.cpp \
    timemodel.cpp \
    timeitem.cpp \
    multipagewidget.cpp \
    cmdline.c \
    alertdisplay.cpp \
    projectlist.cpp
DISTFILES += tlist.ggo
cmdline_c.target = cmdline.c
cmdline_c.commands = gengetopt \
    < \
    tlist.ggo
cmdline_c.depends = cmdline_h
cmdline_h.target = tlist.ggo
QMAKE_EXTRA_TARGETS += cmdline_c \
    cmdline_h
MOC_DIR += .moc
RESOURCES += systray.qrc \
    tlist.qrc
unix:SOURCES += x11.cpp
RC_FILE = pics/tlist.icns
