#ifndef RFINSTALLER_INSTALLER_GLOBAL_H_
#define RFINSTALLER_INSTALLER_GLOBAL_H_

#include <QObject>
#include <QString>
#include <QTranslator>
#include <installengine.h>
#include <sys/utsname.h>

extern const QString 	g_transPrefix;
extern QString 		g_appPath;
extern QString 		g_appResPath;
extern QString 		g_appImgPath;
extern QString 		g_appQmPath;
extern QString 		g_qtQmPath;
extern QTranslator 	g_appTranslator;
extern QTranslator 	g_qtTranslator;
extern Engine* 		g_engine;

extern QString          g_localexml;

enum Page_ID {
    Page_Welcome,
    Page_ID_START=Page_Welcome,
    Page_KeyboardLayout,
    Page_Licence,
    Page_Mach_Type,
    Page_InstallMode,
    Page_ChooseGroup,
    Page_Partition_Mode,
    Page_Partition_Auto, 
    Page_Partition_Simple, 
    Page_Partition_Advanced,
//    Page_Packages_Query,
//    Page_Packages,
    Page_Summary,
    Page_Progress,
    Page_UserAdd,
    Page_Finish,
    Page_ID_END=Page_Finish,
    Page_Error
};

#endif // RFINSTALLER_INSTALLER_GLOBAL_H_
