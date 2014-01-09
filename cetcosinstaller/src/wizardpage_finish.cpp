#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QLocale>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QVariant>
#include <stdlib.h>
#include "installer_global.h"
#include "dialog_postscript.h"
#include "wizardpage_finish.h"
#include "wizardpage_useradd.h"

WizardPage_Finish::WizardPage_Finish(QWidget *parent)
    : QWizardPage(parent)
{
    setFinalPage(true);
    m_topSpacerItem = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_bottomSpacerItem = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_layout = new QVBoxLayout(this);

    m_layout->addItem( m_topSpacerItem );
    m_layout->addItem( m_bottomSpacerItem );

    setLayout(m_layout);
    m_thread = new PostThread();
    m_dialog = new DialogPostscript(this);
    m_dialog->setModal( true );
    connect( m_thread, SIGNAL( poststate(bool, QString) ), this, SLOT( setPostState(bool, QString) ) );
    connect( m_thread, SIGNAL( finished() ), m_dialog, SLOT( accept() ) );
    connect( m_thread, SIGNAL( finished() ), this, SLOT( restorePage() ) );
}

void WizardPage_Finish::initializePage()
{
    setTitle( tr("Finish") );
    setSubTitle( tr("Configuring...") );
    wizard()->button( QWizard::CancelButton )->hide();
    wizard()->button( QWizard::FinishButton )->setEnabled( false );

    // locale
    QString locale = field("locale").toString();
    locale = QLocale(locale).name();
    
    locale = locale + ".UTF-8";
    qDebug() << locale;
    g_engine->cmdSetLang( locale.toLatin1() );

    // grub
    QString pathGrub = field("pathGrub").toString();
    if( !pathGrub.isEmpty() ) {
        qDebug() << "install grub on:" << pathGrub;
        g_engine->cmdSetBootEntry( pathGrub.toLatin1() );
    }

    // useradd
    ( static_cast<WizardPage_UserAdd*>(wizard()->page(Page_UserAdd)) )->setPasswdUser();
    
    //TODO: install new kernel here.

    // start the postscript dialog.
    m_dialog->setLabelText();
    m_dialog->show();
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    m_thread->start();
}

void WizardPage_Finish::restorePage()
{
    QApplication::restoreOverrideCursor();
    if ( m_poststate ) {
	setSubTitle( tr("Congratulation! Enjoy it.") );
    } else {
	setSubTitle( tr("PostInstall is failed.") );
    }
    wizard()->button( QWizard::FinishButton )->setEnabled( true );
}

bool WizardPage_Finish::validatePage()
{
    system("eject");
    system("reboot");
	return true;
}

void WizardPage_Finish::setPostState(bool state, QString errStr)
{
    m_poststate = state;
    if ( !state ) {
	QMessageBox::critical(this, tr("PostInstall Error"), errStr);
    }
    emit exitstate(state);
}

void PostThread::run()
{
    bool ret = g_engine->postscript();
    QString errStr( g_engine->getErr() );
    emit poststate(ret, errStr);
}
