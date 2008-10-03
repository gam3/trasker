HEADERS     = treeitem.h \
              treemodel.h \
	      connection.h \
	      ttcp.h \
	      peermanager.h
RESOURCES   = tlist.qrc
SOURCES     = treeitem.cpp \
              treemodel.cpp \
	      connection.cpp \
	      ttcp.cpp \
	      peermanager.cpp \
              main.cpp
CONFIG  += qt

QT += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/tlist
INSTALLS += target sources
