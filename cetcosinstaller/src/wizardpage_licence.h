#ifndef _WIZARDPAGE_LICENCE_H_
#define _WIZARDPAGE_LICENCE_H_

#include <QWizard>
#include "installer_global.h"

class QGridLayout;
class QRadioButton;
class QSpacerItem;
class QTextBrowser;

class WizardPage_Licence : public QWizardPage
{
    Q_OBJECT
public:
    WizardPage_Licence(QWidget* parent =0);
    void initializePage();

private:
    QTextBrowser	* m_browserLicence;
    QSpacerItem 	* m_spacer;
    QRadioButton 	* m_radiobuttonAccept;
    QRadioButton 	* m_radiobuttonDecline;
    QGridLayout 	* m_layout;
};
#endif // _WIZARDPAGE_LICENCE_H_
