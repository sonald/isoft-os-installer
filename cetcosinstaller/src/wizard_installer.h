#ifndef RFINSTALLER_WIZARD_INSTALLER_H_
#define RFINSTALLER_WIZARD_INSTALLER_H_

#include <QWizard>

class StageIndicator;
class DashBoard;

class WizardInstaller : public QWizard 
{
    Q_OBJECT

public:
    WizardInstaller(QWidget* parent =0);
    void reject();
public slots:
    void setExitState(bool state);
    void updateDashboard(const QPixmap& pix);

private slots:
    void reTranslateUi();
    void updateIndicator(int id);

private:
    bool m_exitstate;
    StageIndicator* _indicator;
    DashBoard* _board;
};
#endif // RFINSTALLER_WIZARD_INSTALLER_H_
