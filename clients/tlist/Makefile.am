
bin_PROGRAMS = tlist

tlist_SOURCES = addproject.cpp \
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
		x11.cpp moc_addproject.cpp \
		moc_auto_select.cpp \
		moc_connection.cpp \
		moc_error.cpp \
		moc_help.cpp \
		moc_mainwindow.cpp \
		moc_mytreeview.cpp \
		moc_notes.cpp \
		moc_setup.cpp \
		moc_treemodel.cpp \
		moc_ttcp.cpp \
		qrc_systray.cpp \
		qrc_tlist.cpp \
		ui_auto_select.h \
		ui_addproject.h \
		cmdline.c


FORMS  = addproject.ui \
         auto_select.ui \
         error.ui \
         help.ui \
         mainwindow.ui \
         notes.ui \
         setup.ui

addproject.o : ui_addproject.h
auto_select.o : ui_auto_select.h
help.o : ui_help.h
error.o : ui_error.h
mainwindow.h : ui_mainwindow.h
setup.o : ui_setup.h
notes.h : ui_notes.h

DIST          = tlist.pro

AM_CXXFLAGS = ${QT4_CFLAGS}
AM_CXXFLAGS += -I/usr/include/qt4/QtWebKit

AM_LDFLAGS = ${QT4_LIBS}
AM_LDFLAGS += -lQtWebKit


ui_%.h :: %.ui
	$(UIC) $< -o $@

moc_%.cpp :: %.h
	$(MOC) $< -o $@

qrc_systray.cpp: systray.qrc pics/active-icon-0.xpm
	/usr/bin/rcc -name systray systray.qrc -o qrc_systray.cpp

qrc_tlist.cpp: tlist.qrc default.txt
	/usr/bin/rcc -name tlist tlist.qrc -o qrc_tlist.cpp

cmdline.c cmdline.h : tlist.ggo
	gengetopt < tlist.ggo

#rm -f moc_addproject.cpp moc_auto_select.cpp moc_connection.cpp moc_error.cpp moc_help.cpp moc_mainwindow.cpp moc_mytreeview.cpp moc_notes.cpp moc_setup.cpp moc_treemodel.cpp moc_ttcp.cpp
#rm -f qrc_systray.cpp qrc_tlist.cpp
#rm -f ui_addproject.h ui_auto_select.h ui_error.h ui_help.h ui_mainwindow.h ui_notes.h ui_setup.h
#rm -f addproject.o auto_select.o connection.o error.o help.o main.o mainwindow.o mytreeview.o notes.o setup.o treeitem.o treemodel.o ttcp.o x11.o moc_addproject.o moc_auto_select.o moc_connection.o moc_error.o moc_help.o moc_mainwindow.o moc_mytreeview.o moc_notes.o moc_setup.o moc_treemodel.o moc_ttcp.o qrc_systray.o qrc_tlist.o
#rm -f *~ core *.core

