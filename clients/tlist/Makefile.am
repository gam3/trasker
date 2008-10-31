
bin_PROGRAMS = tlist

FORMS  = addproject.ui \
         auto_select.ui \
         error.ui \
         help.ui \
         mainwindow.ui \
         notes.ui \
	 timeedit.ui \
         setup.ui

IMAGES = pics/active-clock.gif \
         pics/active-clock.xcf \
	 pics/active-icon-0.xpm \
	 pics/active-icon-1.xpm \
	 pics/active-icon-2.xpm \
	 pics/active-icon-3.xpm \
	 pics/active-icon-4.xpm \
	 pics/active-icon-5.xpm \
	 pics/active-icon-6.xpm \
	 pics/active-icon-7.xpm

tlist_SOURCES = addproject.cpp \
		addproject.h \
		auto_select.cpp \
		auto_select.h \
		cmdline.c \
		cmdline.h \
		conf.h \
		config.h \
		connection.cpp \
		connection.h \
		error.cpp \
		error.h \
		help.cpp \
		help.h \
		main.cpp \
		mainwindow.cpp \
		mainwindow.h \
		mytreeview.cpp \
		mytreeview.h \
		notes.cpp \
		notes.h \
		project.h \
		settings.h \
		setup.cpp \
		setup.h \
		timeedit.cpp \
		timeedit.h \
		timeitem.cpp \
		timeitem.h \
		timemodel.cpp \
		timemodel.h \
		treeitem.cpp \
		treeitem.h \
		treemodel.cpp \
		treemodel.h \
		ttcp.cpp \
		ttcp.h \
		x11.cpp \
		systray.qrc \
		tlist.qrc \
		$(FORMS) $(IMAGES)

nodist_tlist_SOURCES =  ui_addproject.h \
			ui_auto_select.h \
			ui_error.h \
			ui_help.h \
			ui_mainwindow.h \
			ui_notes.h \
			ui_setup.h \
			ui_timeedit.h \
			moc_addproject.cpp \
			moc_auto_select.cpp \
			moc_connection.cpp \
			moc_error.cpp \
			moc_help.cpp \
			moc_mainwindow.cpp \
			moc_mytreeview.cpp \
			moc_notes.cpp \
			moc_setup.cpp \
			moc_timeedit.cpp \
			moc_timemodel.cpp \
			moc_treemodel.cpp \
			moc_ttcp.cpp \
			qrc_systray.cpp


addproject.h : ui_addproject.h
auto_select.h : ui_auto_select.h
error.h : ui_error.h
help.h : ui_help.h
mainwindow.h : ui_mainwindow.h
notes.h : ui_notes.h
setup.h : ui_setup.h
timeedit.h : ui_timeedit.h

DIST          = tlist.pro

AM_CXXFLAGS = ${QT4_CFLAGS}
AM_CXXFLAGS += -I/usr/include/qt4/QtWebKit

AM_LDFLAGS = ${QT4_LIBS}
AM_LDFLAGS += -lQtWebKit

addproject.o : addproject.h addproject.cpp
auto_select.o : auto_select.h auto_select.cpp
connection.o : connection.h connection.cpp setup.h
help.o : help.cpp help.h
mainwindow.o : mainwindow.cpp mainwindow.h notes.h help.h setup.h auto_select.h addproject.h error.h  timeedit.h
main.o : mainwindow.h cmdline.h
timeedit.o : timeedit.cpp timeedit.h

ui_%.h : %.ui
	$(UIC) $< -o $@

moc_%.cpp : %.h
	$(MOC) $< -o $@

qrc_%.cpp : %.qrc
	rcc -name $* $< -o $@

cmdline.c cmdline.h : tlist.ggo
	gengetopt < tlist.ggo

clean-local:
	-rm -rf ui_*.h moc_*.cpp qrc_*.cpp
	-rm cmdline.c cmdline.h

