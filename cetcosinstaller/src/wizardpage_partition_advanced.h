#ifndef RFINSTALLER_WIZARDPAGE_PARTITION_ADVANCED_
#define RFINSTALLER_WIZARDPAGE_PARTITION_ADVANCED_

#include <QWizardPage>
#include <diskswidget.h>
#include "installer_global.h"
#include "wizardpage_partition.h"

class QString;
class QVBoxLayout;


class WizardPage_Partition_Advanced : public WizardPage_Partition
{
    Q_OBJECT
public:
    WizardPage_Partition_Advanced(QWidget *parent =0);
    void initializePage();
    void cleanupPage();
    bool validatePage();

    const QString finalPartInfo();
    const QString warningInfo();
    const QString rootPartPath();
    void writeConf();

private:
    DisksWidget* m_advanced;
    QVBoxLayout* m_layout;
};
#endif // RFINSTALLER_WIZARDPAGE_PARTITION_ADVANCED_
