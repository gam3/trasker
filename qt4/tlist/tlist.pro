MAKEFILE = makefile

FORMS       = mainwindow.ui \
	      addproject.ui \
	      auto_select.ui \
	      notes.ui \
	      error.ui \
	      help.ui

HEADERS     = mainwindow.h \
              treeitem.h \
              treemodel.h \
	      connection.h \
	      ttcp.h \
	      notes.h \
	      auto_select.h \
	      addproject.h \
              help.h \
              error.h \
	      mytreeview.h

RESOURCES   = tlist.qrc systray.qrc

SOURCES     = mainwindow.cpp \
	      treeitem.cpp \
              treemodel.cpp \
	      connection.cpp \
	      ttcp.cpp \
	      mytreeview.cpp \
	      x11.cpp \
	      notes.cpp \
	      auto_select.cpp \
	      addproject.cpp \
              help.cpp \
              error.cpp \
              main.cpp
CONFIG += qt

CONFIG += debug

QT += network

QT += webkit

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
INSTALLS += target sources

dox.target = doc
dox.commands = doxygen Doxyfile; \
    test -d html/images || mkdir html/images; \
    cp images/* html/images
dox.depends =

QMAKE_EXTRA_UNIX_TARGETS += dox
