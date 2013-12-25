#ifndef RFINSATLLER_WIZARDPAGE_PARTITION_AUTO_H_
#define RFINSATLLER_WIZARDPAGE_PARTITION_AUTO_H_

#include <QtGui>
#include "installer_global.h"
#include "wizardpage_partition.h"

class PartitionAutoMode;
class Device;

class WizardPage_Partition_Auto : public WizardPage_Partition
{
    Q_OBJECT
public:
    WizardPage_Partition_Auto( QWidget* parent=0 );
    ~WizardPage_Partition_Auto();
    const QString finalPartInfo();
    const QString warningInfo();
    const QString rootPartPath();
    void writeConf();

private slots:
    void changeDestinationDisk(const QString &disk);

private:
    PartitionAutoMode* m_automode;

    QVBoxLayout *m_layout;
    QSignalMapper *m_sigMap;

    void addDisk(Device *disk);
    // call partDisk to get result before call funcations above.
    bool autoDiskPart();

};
#endif // RFINSATLLER_WIZARDPAGE_PARTITION_AUTO_H_
