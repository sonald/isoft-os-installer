#ifndef RFINSTALLER_WIZARD_INSTALLER_H_
#define RFINSTALLER_WIZARD_INSTALLER_H_

#include <QWizard>
#include <QtGui>

class StageIndicator;
class DashBoard;

class WizardInstaller : public QWizard 
{
    Q_OBJECT

public:
    WizardInstaller(QWidget* parent =0);
    void reject();
    void doPacmanInit();
    void copyGpgKeys();

public slots:
    void setExitState(bool state);
    void updateDashboard(const QPixmap& pix);

signals:
    void pacmanInitDone(bool success);

private slots:
    void reTranslateUi();
    void updateIndicator(int id);
    void onProcessFinished(int, QProcess::ExitStatus);

private:
    bool m_exitstate;
    StageIndicator* _indicator;
    DashBoard* _board;
    QProcess* _pacmanInitProc;
    QProcess* _pacmanPopulateProc;
    bool _pacmanInitDone;
};
#endif // RFINSTALLER_WIZARD_INSTALLER_H_
