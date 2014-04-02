#include <QtGui>
#include <stdlib.h>
#include "installer_global.h"
#include "dialog_postscript.h"
#include "wizardpage_finish.h"
#include "wizardpage_useradd.h"

    WizardPage_Finish::WizardPage_Finish(QWidget *parent)
: QWizardPage(parent)
{
    m_poststate = false;

    m_topSpacerItem = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_bottomSpacerItem = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_layout = new QVBoxLayout(this);

    m_layout->addItem( m_topSpacerItem );
    m_layout->addItem( m_bottomSpacerItem );

    setLayout(m_layout);
}

void WizardPage_Finish::initializePage()
{
    setFinalPage(true);

    setTitle( tr("Finish") );
    setSubTitle( tr("Configuring...") );
    wizard()->button( QWizard::CancelButton )->hide();

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


    // start the postscript dialog.
    m_thread = new PostThread();
    connect( m_thread, SIGNAL( poststate(bool, QString) ), this, SLOT( setPostState(bool, QString) ) );
    connect( m_thread, SIGNAL( finished() ), this, SLOT( restorePage() ) );

    m_dialog = new DialogPostscript(this);
    m_dialog->setModal( true );
    connect( m_thread, SIGNAL( finished() ), m_dialog, SLOT( accept() ) );

    m_dialog->setLabelText();
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QTimer::singleShot(0, this, SLOT(start()));
}

void WizardPage_Finish::start()
{
    wizard()->button(QWizard::FinishButton)->setEnabled(false);
    QTimer::singleShot(40, m_thread, SLOT(start()));
    //m_thread->start();
    m_dialog->show();
}

void WizardPage_Finish::restorePage()
{
    QApplication::restoreOverrideCursor();
    if ( m_poststate ) {
        setSubTitle( tr("Congratulation! Enjoy it.") );
    } else {
        setSubTitle( tr("PostInstall is failed.") );
    }

    emit completeChanged();
    //wizard()->button( QWizard::FinishButton )->setEnabled( true );
}

bool WizardPage_Finish::validatePage()
{
    system("systemctl reboot");
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

bool WizardPage_Finish::isComplete()
{
    return m_poststate;
}

void PostThread::run()
{
    bool ret = g_engine->postscript();
    QString errStr( g_engine->getErr() );
    emit poststate(ret, errStr);
}
