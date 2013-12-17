#ifndef RFINSTALLER_WIZARDPAGE_PARTITION_SIMPLE_H_
#define RFINSTALLER_WIZARDPAGE_PARTITION_SIMPLE_H_

#include <QVBoxLayout>
#include <QWizardPage>
#include <diskswidget.h>
#include "installer_global.h"
#include "wizardpage_partition.h"

class WizardPage_Partition_Simple : public WizardPage_Partition
{
    Q_OBJECT
public:
    WizardPage_Partition_Simple(QWidget *parent =0);
    void initializePage();
    void cleanupPage();
    bool isComplete() const;
    bool validatePage();

    const QString finalPartInfo();
    const QString warningInfo();
    const QString rootPartPath();
    void writeConf();

public slots:
    void getCurPartInfo( QString dev, QString parttype, QString fstype );

private:
    DisksWidget* m_simple;
    QVBoxLayout* m_layout;
    bool m_canInstall;
};
#endif // RFINSTALLER_WIZARDPAGE_PARTITION_SIMPLE_H_
