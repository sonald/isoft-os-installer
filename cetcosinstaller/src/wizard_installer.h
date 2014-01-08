#ifndef RFINSTALLER_WIZARD_INSTALLER_H_
#define RFINSTALLER_WIZARD_INSTALLER_H_

#include <QWizard>

class WizardInstaller : public QWizard 
{
    Q_OBJECT

public:
    WizardInstaller(QWidget* parent =0);
    void reject();
public slots:
    void setExitState(bool state);

private slots:
    void reTranslateUi();

private:
    bool m_exitstate;
};
#endif // RFINSTALLER_WIZARD_INSTALLER_H_
