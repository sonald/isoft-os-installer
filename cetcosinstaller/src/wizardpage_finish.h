#ifndef RFINSTALLER_WIZARDPAGE_FINISH_H_
#define RFINSTALLER_WIZARDPAGE_FINISH_H_

#include <QWizardPage>
#include <QThread>

class QCheckBox;
class QSpacerItem;
class QString;
class QVBoxLayout;
class QThread;
class DialogPostscript;
class PostThread;

class WizardPage_Finish : public QWizardPage
{
Q_OBJECT
    
public:
    WizardPage_Finish(QWidget * parent =0);
    void initializePage();
    bool validatePage();
    bool isComplete();

public slots:
    void restorePage();
    void setPostState(bool state, QString errStr);
signals:
    void exitstate(bool state);

private:
    QVBoxLayout	*m_layout;
    QSpacerItem *m_topSpacerItem;
    QSpacerItem *m_bottomSpacerItem;
    PostThread  *m_thread;
    DialogPostscript *m_dialog;
    bool m_poststate;
};

class PostThread : public QThread
{
Q_OBJECT
    
public:
    void run();
signals:
    void poststate(bool state, QString errStr);
};
#endif // RFINSTALLER_WIZARDPAGE_FINISH_H_
