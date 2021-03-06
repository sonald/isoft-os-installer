#include <QAbstractButton>
#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QSpacerItem>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <cassert>
#include <cstdlib>
#include <time.h>
#include <installengine.h>
#include "wizard_installer.h"
#include "wizardpage_progress.h"

// s_this: static object which store the this pointer of thread.
// s_setProgressByEngine called by g_engine, 
// the function and pointer should be static.
InstallThread* InstallThread::s_this = NULL;
void InstallThread::s_setProgressByEngine(Engine::Stage stage, int value)
{
    emit s_this->updateProgress(stage, value);
}

// construct the thread, init the static data.
InstallThread::InstallThread()
{
    s_this = this;
}

// run engine, use the static function as the call-back function.
// when installation is over, emit the signal to GUI.
void InstallThread::run()
{
    bool ret = g_engine->install( InstallThread::s_setProgressByEngine );
    QString errStr( g_engine->getErr() );
    emit endProgress(ret, errStr);
}

WizardPage_Progress::WizardPage_Progress(QWidget *parent)
    : QWizardPage(parent)
{
    // construct GUI elements.
    m_picture = new QLabel(this);
    m_time_label = new QLabel(this);
    m_bar = new QProgressBar(this);
    m_picture->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_picture);
    m_layout->addWidget(m_bar);
    m_layout->addWidget(m_time_label);
    setLayout(m_layout);
    setCommitPage(true);

    // load the pictures.
    QDir dir( g_appImgPath );
    qDebug() << g_appImgPath;
    m_picsNameList = dir.entryList( QStringList("installer-0*.jpg") );
    assert( !m_picsNameList.isEmpty() );

    // set the timer of picture.
    m_timerPic = new QTimer(this);
    connect(m_timerPic, SIGNAL(timeout()), this, SLOT(updatePic()));

    m_timer_progress = new QTimer(this);
    connect(m_timer_progress, SIGNAL(timeout()), this, SLOT(updateProgress()));

    // hard code the range ( 0, 100 )
    m_max_value = 100;
    m_min_value = 0;

    // create the thread, and set connect.
    m_thread = new InstallThread();
    qRegisterMetaType<Engine::Stage>("Engine::Stage");
    connect( m_thread, SIGNAL( updateProgress(Engine::Stage, int) ), 
            this, SLOT( updateProgress(Engine::Stage, int) ) );
    connect( m_thread, SIGNAL( endProgress(bool, QString) ), this, SLOT( endProgress(bool, QString) ) );
}

void WizardPage_Progress::initializePage()
{
    qDebug() << "initializePage progress.";

    setTitle( tr("Installation Progress") );
    setSubTitle( tr("Please wait for a while. Enjoy a cup of coffee maybe a good idea.") );

    m_bar->setRange( m_min_value, m_max_value );
    m_bar->setValue( m_min_value );
//    m_bar->setMaximum(0);

    wizard()->button( QWizard::CancelButton )->setEnabled( false );
    m_end = false;

    // set the beginning picture.
    QString locale = field("locale").toString();
    QPixmap pic( g_appImgPath + "/installer-begin-" + locale + ".jpg");
    m_picture->setPixmap(pic);
    m_indexPic = 0;

    // set the interval of timer of picture to 8 seconds.
    m_timerPic->setInterval(8000);
    m_timer_progress->setInterval(1100);
    m_timer_progress->start();
    // start the progress after 1 second.
    QTimer::singleShot( 1000, this, SLOT(startProgress()) );
}

void WizardPage_Progress::updatePic()
{
    m_indexPic = ( m_indexPic + 1 ) % m_picsNameList.size();
    qDebug() << ( g_appImgPath + "/" + m_picsNameList.at( m_indexPic ) );
    QPixmap pic( g_appImgPath + "/" + m_picsNameList.at( m_indexPic ) );
    if (pic.isNull()) {
        qDebug() << m_picsNameList[m_indexPic] << "is null";
        return;
    }
    m_picture->setPixmap(pic);
    WizardInstaller *installer = qobject_cast<WizardInstaller*>(wizard());
    installer->updateDashboard(pic);
}

void WizardPage_Progress::updateProgress()
{
    QString msg;
    switch(m_stage) {
        case Engine::DISKSPACE:
            msg = tr("checking available disk space...");
            break;
        case Engine::CONFLICTS:
            msg = tr("checking for file conflicts...");
            break;
        case Engine::INTEGRITY:
            msg = tr("checking package integrity...");
            break;
        case Engine::KEYRING:
            msg = tr("checking keys in keyring...");
            break;
        case Engine::LOAD:
            msg = tr("loading package files...");
            break;

        case Engine::ADD:
        case Engine::UPGRADE: /* passthrough */
        default: 
            m_time_elapse = m_time_elapse.addSecs(1);
            msg = tr("installing packages, elapsed: ") +  m_time_elapse.toString("hh:mm:ss");
            break;
            
    }
    m_time_label->setText(msg);
}

int WizardPage_Progress::nextId() const
{
    return QWizardPage::nextId();
}

void WizardPage_Progress::updateProgress(Engine::Stage stage, int value)
{
    m_stage = stage;
    m_bar->setValue( value );
}

void WizardPage_Progress::startProgress()
{
    printf("startProgress\n");
    srand( unsigned( time( NULL ) ) );
    m_indexPic = static_cast<int> ( m_picsNameList.size() * ( rand() / ( RAND_MAX + 1.0 ) ) );
    m_timerPic->start();
    m_thread->start();
}

void WizardPage_Progress::endProgress(bool state, QString error)
{
    printf("endProgress\n");
    printf("state %d\n", int(state));
    m_timerPic->stop();
    m_timer_progress->stop();
    emit exitstate( state );
    m_bar->setRange( m_min_value, m_max_value );
    m_bar->setValue( m_max_value );
    if ( !state ) {
        QMessageBox::critical(this, tr("Installation Error"), error);
        wizard()->setButtonText( QWizard::CancelButton, tr("&Finish") );
        wizard()->button( QWizard::CancelButton )->setEnabled( true );
        m_end = false;
    } else {
        QString locale = field("locale").toString();
        QPixmap pic( g_appImgPath + "/installer-end-" + locale + ".jpg");
        m_picture->setPixmap( pic );
        m_end = true;
    }
    wizard()->setOption(QWizard::HaveCustomButton2, false);
    emit completeChanged();
}

bool WizardPage_Progress::isComplete() const
{
    return m_end;
}
