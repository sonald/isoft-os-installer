#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include <cstdio>
#include <unistd.h>
#include <installengine.h>

#include "dialog_root.h"
#include "installer_global.h"
#include "wizard_installer.h"

// global definitions.
const QString   g_transPrefix = "installer";
QString 	g_appPath;
QString 	g_appResPath;
QString 	g_qtQmPath;
QString 	g_appQmPath;
QString 	g_appImgPath;
QTranslator 	g_appTranslator;
QTranslator 	g_qtTranslator;
Engine* 	g_engine;

QString         g_localexml = "/etc/rflocale.xml";

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if ( getuid() != 0 ) { // permission of root required.
    	DialogRoot root_dialog;
	root_dialog.exec();
	return -1;
    } else {
	// unique the application.
	char pid[16];
	FILE* fpPID = fopen( "/tmp/rfinstaller.pid", "r" );
	if ( fpPID != NULL ) { // pid file is existed.
	    fscanf( fpPID, "%s\n", pid );
	    char path[PATH_MAX];
	    sprintf( path, "/proc/%s/cmdline", pid );
	    qDebug() << path;

	    char progname[PATH_MAX];
	    FILE* fpProg = fopen( path, "r" );
	    if ( fpProg != NULL ) { // process is existed.
		fscanf( fpProg, "%s\n", progname );
		qDebug() << progname;
		if ( !strcmp( "rfinstaller", basename(progname) ) ) { // and is rfinstaller 
		    qDebug() << "rfinstaller is already running.";
		    exit(0);
		} // not the rfinstaller.
		fclose( fpProg );
	    } else { // process is not existed. 
	    qDebug() << "process not exist.";
	    }
	} // file is not existed.
	fpPID = fopen( "/tmp/rfinstaller.pid", "w+" );
	fprintf( fpPID, "%d\n", getpid() );
	fclose( fpPID );
	
	// get the env of app, and load the translate files.
	QString localeName = QLocale::system().name();
    
	g_qtQmPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

	g_appPath = app.applicationDirPath();
	g_appResPath = g_appPath + "/../share/rfinstaller";
	g_appQmPath = g_appResPath + "/translations";
	g_appImgPath = g_appResPath + "/images";
    
	g_appTranslator.load( g_transPrefix + "_" + localeName, g_appQmPath );
	g_qtTranslator.load( "qt_" + localeName, g_qtQmPath );

	app.installTranslator(&g_appTranslator);
	app.installTranslator(&g_qtTranslator);

	// /etc/install.conf work as a flag. if exists, write the conf to file in disk.
	// else just install.
	QFile confflag("/etc/install.conf");
	if ( confflag.exists() )
	    g_engine = Engine::instance(Engine::WriteConf, "/tmp/dt7_conf.xml");
	else
	    g_engine = Engine::instance(Engine::Install);

	// show the app.
	WizardInstaller wizard_installer; 
	wizard_installer.show();

	// run the app.
	int ret;
	ret = app.exec();

	// clean the app.
	delete g_engine;
	remove("/tmp/rfinstaller.pid");

	return ret;
    }
}
