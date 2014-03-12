#ifndef _wizardpage_install_mode_h
#define _wizardpage_install_mode_h    

#include <QtGui>

namespace Ui {
class WizardPage_install_mode;
}

class WizardPage_installmode: public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(int requiredSize READ requiredSize WRITE setRequiredSize NOTIFY requiredSizeChanged)

public:
    explicit WizardPage_installmode(QWidget *parent = 0);
    void initializePage();
    ~WizardPage_installmode();

    int nextId() const;
    bool validatePage();
    void cleanupPage();

    int requiredSize() const { return m_requiredSize; }
    void setRequiredSize(int val) { m_requiredSize = val; emit requiredSizeChanged(val); }


signals:
    void requiredSizeChanged(int);

private:
    Ui::WizardPage_install_mode *ui;
    int m_requiredSize;
};

#endif
