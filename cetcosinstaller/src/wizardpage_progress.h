#ifndef RFINSTALLER_WIZARDPAGE_PROGRESS_H_
#define RFINSTALLER_WIZARDPAGE_PROGRESS_H_

#include <QStringList>
#include <QThread>
#include <QWizardPage>
#include <QTime>
#include "installer_global.h"

class InstallThread;
class QLabel;
class QProgressBar;
class QSpacerItem;
class QTimer;
class QVBoxLayout;

class WizardPage_Progress : public QWizardPage
{
Q_OBJECT

public:
    WizardPage_Progress(QWidget* parent =0);
    void initializePage();
    int nextId() const;
    bool isComplete() const;

public slots:
    void updateProgress(Engine::Stage, int value);
    void startProgress();
    void endProgress(bool state, QString error);

signals:
    void exitstate(bool state);

private slots:
    void updatePic();
    void updateProgress();

private:
    QLabel*		m_picture;
    QProgressBar* 	m_bar;
    QVBoxLayout*	m_layout;
    
    QStringList		m_picsNameList;
    int			m_indexPic;
    QTimer*             m_timerPic;
    QTimer*             m_timer_progress;
    QTime		m_time_elapse;
    QLabel*		m_time_label;
	
    bool                m_end;
    int 		m_max_value;
    int 		m_min_value;
    InstallThread*	m_thread;
    Engine::Stage m_stage;
};

class InstallThread : public QThread 
{
    Q_OBJECT
public:
    InstallThread();
    static void s_setProgressByEngine(Engine::Stage, int);
    static InstallThread* s_this;
protected:
    void run();

signals:
    void updateProgress(Engine::Stage, int);
    void endProgress(bool, QString);
};
#endif // RFINSTALLER_WIZARDPAGE_PROGRESS_H_
