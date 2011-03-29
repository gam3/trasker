SOURCES = test.cpp
CONFIG  += qtestlib

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
#sources.files = $$SOURCES *.pro
#sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
#INSTALLS += target sources

QT += network

HEADERS += connection.h ttcp.h

LIBS += -L/home/gam3/src/trasker/clients/qt/ttcp -lttcp
