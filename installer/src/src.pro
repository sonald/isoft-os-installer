TEMPLATE = app
TARGET = rfinstaller
CONFIG += link_pkgconfig
PKGCONFIG = glib-2.0
DEPENDPATH += . 
INCLUDEPATH += . /usr/include/parted++ ../games
LIBS += -lparted -lparted++ -lglib-2.0 -linstallengine -ldiskswidget -L../games -lrfinsgame
QT += xml

# Input
HEADERS += dialog_reject.h \
           dialog_root.h \
	   dialog_postscript.h \
           dialog_summary.h \
           installer_global.h \
           parser_locale.h \
           wizard_installer.h \
           wizardpage_error.h \
           wizardpage_finish.h \
           wizardpage_licence.h \
	   wizardpage_partition.h\
           wizardpage_partition_advanced.h \
           wizardpage_progress.h \
           wizardpage_summary.h \
           wizardpage_welcome.h	\
           wizardpage_mach_type.h

SOURCES += dialog_reject.cpp \
           dialog_root.cpp \
	   dialog_postscript.cpp \
           dialog_summary.cpp \
           main.cpp \
           parser_locale.cpp \
           wizard_installer.cpp \
           wizardpage_finish.cpp \
           wizardpage_licence.cpp \
           wizardpage_partition_advanced.cpp \
           wizardpage_progress.cpp \
           wizardpage_summary.cpp \
           wizardpage_welcome.cpp \
           wizardpage_mach_type.cpp

target.path = /usr/bin
INSTALLS += target
