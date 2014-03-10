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
#include <QDebug>
#include <QtGui/QMessageBox>

WizardInstaller::WizardInstaller(QWidget* parent) 
	: QWizard(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setOption(NoBackButtonOnStartPage);
    setOption(NoBackButtonOnLastPage);
//    setOption(HaveCustomButton1);
    setWizardStyle(MacStyle);

    // create all wizard page now.
    WizardPage_Welcome * 	page_welcome		= new WizardPage_Welcome;
//    WizardPage_Licence * 	page_licence 		= new WizardPage_Licence;

//    WizardPage_Partition_Mode*		page_partition_mode = new WizardPage_Partition_Mode;
//    WizardPage_Partition_Auto*		page_partition_auto = new WizardPage_Partition_Auto;
//    WizardPage_Partition_Simple*	page_partition_simple = new WizardPage_Partition_Simple;
    WizardPage_installmode * page_install_mode = new WizardPage_installmode;
    WizardPage_Partition_Advanced * 	page_partition_advanced = new WizardPage_Partition_Advanced;

    WizardPage_chooseGroup * page_choosegroup = new WizardPage_chooseGroup;
    WizardPage_Summary * 	page_summary		= new WizardPage_Summary;
    WizardPage_Progress *	page_progress		= new WizardPage_Progress;

    WizardPage_UserAdd * page_useradd = new WizardPage_UserAdd;
    WizardPage_Finish * page_finish = new WizardPage_Finish;
    
    setPage(Page_Welcome, page_welcome);
    setPage(Page_InstallMode, page_install_mode);
//    setPage(Page_Licence, page_licence);
//    setPage(Page_Mach_Type, page_mach_type);
//    setPage(Page_Partition_Mode, page_partition_mode);
//    setPage(Page_Partition_Auto, page_partition_auto);
//    setPage(Page_Partition_Simple, page_partition_simple);
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

    // set the pixmap of wizard: watermark, logo, banner.
    // logo used in modern and classic style.
    // background used in mac style.
    // watermark used in modern and classic style.
    // banner used in modern style.

    //QPixmap watermark( g_appImgPath + "/watermark1.png");
    //setPixmap(QWizard::WatermarkPixmap, watermark);
    //QPixmap banner("./images/banner.png");
    //setPixmap(QWizard::BannerPixmap, banner);
    //QPixmap logo( g_appImgPath + "/installer-logo.png");
    //setPixmap(QWizard::LogoPixmap, logo);
    
    QPixmap background( g_appImgPath + "/installer-background.png" );
    setPixmap( QWizard::BackgroundPixmap, background );
}

// retranslate text of UI element.
void WizardInstaller::reTranslateUi()
{
    setWindowTitle( tr("Installer Wizard") ); 
    
    setButtonText( QWizard::BackButton, tr("< &Back") );
    setButtonText( QWizard::NextButton, tr("&Next >") );
    setButtonText( QWizard::CommitButton, tr("&Next >") );
    setButtonText( QWizard::FinishButton, tr("&Finish") );
    setButtonText( QWizard::CancelButton, tr("&Cancel") );
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
