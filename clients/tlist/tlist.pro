# #####################################################################
# 
# #####################################################################

MAKEFILE = qmakefile
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt \
    debug
QT += network \
    webkit

# Input
HEADERS += addproject.h \
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
    setup.h \
    treeitem.h \
    treemodel.h \
    ttcp.h \
    cmdline.h timeedit.h \
    timemodel.h  timeitem.h

FORMS += addproject.ui \
    auto_select.ui \
    error.ui \
    help.ui \
    mainwindow.ui \
    notes.ui \
    setup.ui timeedit.ui

SOURCES += addproject.cpp \
    auto_select.cpp \
    connection.cpp \
    error.cpp \
    help.cpp \
    main.cpp \
    mainwindow.cpp \
    mytreeview.cpp \
    notes.cpp \
    setup.cpp \
    treeitem.cpp \
    treemodel.cpp \
    ttcp.cpp \
    cmdline.c timeedit.cpp \
    timemodel.cpp timeitem.cpp

MOC_DIR += .moc
RESOURCES += systray.qrc \
    tlist.qrc
unix:SOURCES += x11.cpp
