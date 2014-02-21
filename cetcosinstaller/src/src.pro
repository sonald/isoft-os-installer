TEMPLATE = app
TARGET = cetcosinstaller
CONFIG += link_pkgconfig
PKGCONFIG = glib-2.0
DEPENDPATH += . 
INCLUDEPATH += . /usr/include/parted++ 
LIBS += -lparted -lparted++ -lglib-2.0 -linstallengine -ldiskswidget
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
           wizardpage_partition_auto.h \
           wizardpage_partition_mode.h \
           partition_automode.h \
           wizardpage_progress.h \
           wizardpage_summary.h \
           wizardpage_welcome.h	\
           wizardpage_mach_type.h \
           wizardpage_packages_query.h \
           wizardpage_useradd.h \
           wizardpage_choosegroup.h \
           archinfo.h

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
           wizardpage_partition_auto.cpp \
           wizardpage_partition_mode.cpp \
           partition_automode.cpp \
           wizardpage_progress.cpp \
           wizardpage_summary.cpp \
           wizardpage_welcome.cpp \
           wizardpage_mach_type.cpp \
           wizardpage_packages_query.cpp \
           wizardpage_useradd.cpp \
           wizardpage_choosegroup.cpp \
           archinfo.cpp

target.path = /usr/bin
INSTALLS += target

FORMS += \
    wizardpage_choosegroup.ui
