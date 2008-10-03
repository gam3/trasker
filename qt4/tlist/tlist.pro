FORMS       = mainwindow.ui
HEADERS     = mainwindow.h \
              treeitem.h \
              treemodel.h \
	      connection.h \
	      ttcp.h
RESOURCES   = tlist.qrc systray.qrc
SOURCES     = mainwindow.cpp \
	      treeitem.cpp \
              treemodel.cpp \
	      connection.cpp \
	      ttcp.cpp \
              main.cpp
CONFIG  += qt

CONFIG += debug

QT += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
INSTALLS += target sources
