#include "wizard_installer.h"
#include "wizardpage_welcome.h"
#include "wizardpage_licence.h"
#include "wizardpage_mach_type.h"
#include "wizardpage_partition_mode.h"
#include "wizardpage_partition_auto.h"
#include "wizardpage_partition_simple.h"
#include "wizardpage_install_mode.h"
#include "wizardpage_partition_advanced.h"
#include "wizardpage_choosegroup.h"
#include "wizardpage_summary.h"
#include "wizardpage_progress.h"
#include "wizardpage_useradd.h"
#include "wizardpage_finish.h"
#include "installer_global.h"
#include "dialog_reject.h"
#include "stage_indicator.h"
#include "dashboard.h"
#include <QDebug>
#include <QtGui/QMessageBox>

WizardInstaller::WizardInstaller(QWidget* parent) 
	: QWizard(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setOption(NoBackButtonOnStartPage);
    setOption(NoBackButtonOnLastPage);
    setWizardStyle(MacStyle);

    // create all wizard page now.
    WizardPage_Welcome * 	page_welcome		= new WizardPage_Welcome;
    WizardPage_Licence * 	page_licence 		= new WizardPage_Licence;

    WizardPage_installmode * page_install_mode = new WizardPage_installmode;
    WizardPage_Partition_Advanced * 	page_partition_advanced = new WizardPage_Partition_Advanced;

    WizardPage_chooseGroup * page_choosegroup = new WizardPage_chooseGroup;
    WizardPage_Summary * 	page_summary		= new WizardPage_Summary;
    WizardPage_Progress *	page_progress		= new WizardPage_Progress;

    WizardPage_UserAdd * page_useradd = new WizardPage_UserAdd;
    WizardPage_Finish * page_finish = new WizardPage_Finish;
    
    setPage(Page_Welcome, page_welcome);
    setPage(Page_Licence, page_licence);
    setPage(Page_InstallMode, page_install_mode);
    setPage(Page_Partition_Advanced, page_partition_advanced);
    setPage(Page_ChooseGroup, page_choosegroup);
    setPage(Page_Summary, page_summary);
    setPage(Page_Progress, page_progress);
    setPage(Page_UserAdd, page_useradd);
    setPage(Page_Finish, page_finish);
    
    connect( page_welcome, SIGNAL( localeChanged() ), this, SLOT( reTranslateUi() ) );
    connect( page_progress, SIGNAL( exitstate(bool) ), this, SLOT( setExitState(bool) ) );
    connect( page_finish, SIGNAL( exitstate(bool) ), this, SLOT( setExitState(bool) ) );
    
    m_exitstate = true;
    reTranslateUi();
    QPixmap background( g_appImgPath + "/installer-background.png" );
    setPixmap( QWizard::BackgroundPixmap, background );

    _indicator = new StageIndicator(this);
    _indicator->move(80, 80);
    _indicator->show();
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(updateIndicator(int)));

    _board = new DashBoard(0);
    _board->move(0, 0);
    _board->show();
    _board->lower();
}

void WizardInstaller::updateDashboard(const QPixmap& pix)
{
    _board->updateWith(pix);    
}

void WizardInstaller::updateIndicator(int id)
{
    static int previd = -1;
    qDebug() << "change id to " << id;
    if (id > previd) {
        if (id != Page_ChooseGroup)
            _indicator->nextStage();
    } else if (id < previd) {
        if (!(previd == Page_ChooseGroup && id == Page_InstallMode))
            _indicator->prevStage();
    }
    previd = id;
}

// retranslate text of UI element.
void WizardInstaller::reTranslateUi()
{
    setWindowTitle( tr("Installer Wizard") ); 
    
    setButtonText(QWizard::BackButton, tr("< &Back"));
    setButtonText(QWizard::NextButton, tr("&Next >"));
    setButtonText(QWizard::CommitButton, tr("&Next >"));
    setButtonText(QWizard::FinishButton, tr("&Finish"));
    setButtonText(QWizard::CancelButton, tr("&Cancel"));
}

// exitstate: false stand for error exit.
void WizardInstaller::setExitState(bool state)
{
    m_exitstate = state ;
}

void WizardInstaller::reject()
{
    int curId = currentId();

    if( !m_exitstate ) { // error
        QWizard::reject();
    } else if ( curId == Page_Finish ) {
        QWizard::accept();
    } else {
        DialogReject reject(this);

        if ( curId >= Page_Welcome && curId <= Page_Summary ) {
            reject.setWarning( tr("The installation is not start yet, anything is not "
                                  "happened on your computer, you can quit the installer safely in this moment.") );
        } else if ( ( curId == Page_Progress ) || ( curId == Page_Finish ) ) {
            reject.setWarning( tr("The system already had been installed but not configured."
                                  "The system will be unusable if you quit now.") );
        }

        int ret = reject.exec();
        if ( ret == QDialog::Accepted ) {
            system("systemctl reboot");
            QWizard::reject();
        }
    }
}

void WizardInstaller::doPacmanInit()
{
    //pacman-key --init && pacman-key --populate archlinux
    _pacmanInitDone = false;
    _pacmanPopulateProc = NULL;

    _pacmanInitProc = new QProcess;
    connect(_pacmanInitProc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    _pacmanInitProc->start("pacman-key --gpgdir /tmp/gnupg --init");

}

void WizardInstaller::copyGpgKeys()
{
    static bool keysCopied = false;

    while (!_pacmanInitDone) {
        qDebug() << "busy wait here";
        qApp->processEvents();
    }

    if (keysCopied) {
        qDebug() << "keys already copied, escape";
        return;
    }

    qDebug() << "do copyGpgKeys";
    string cmd("cp -ar /tmp/gnupg /tmp/rootdir/etc/pacman.d/gnupg");
    system(cmd.c_str());

    keysCopied = true;
    delete _pacmanInitProc;
    delete _pacmanPopulateProc;
}

void WizardInstaller::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    static int stage = 1;
    if (stage == 1) {
        qDebug() << "key init done";
        _pacmanPopulateProc = new QProcess;
        connect(_pacmanPopulateProc, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
        _pacmanPopulateProc->start("pacman-key --gpgdir /tmp/gnupg --populate archlinux");
    }

    if (stage == 2) {
        _pacmanInitDone = true;
        qDebug() << "key populate done";
    }
    stage++;
}
