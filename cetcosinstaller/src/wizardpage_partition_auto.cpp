#include <QDebug>
#include "partition_automode.h"
#include "wizardpage_partition_auto.h"


WizardPage_Partition_Auto::WizardPage_Partition_Auto(QWidget* parent)
    : WizardPage_Partition(parent)
{
    m_automode = new PartitionAutoMode;
}

WizardPage_Partition_Auto::~WizardPage_Partition_Auto()
{
    if ( m_automode )
	delete m_automode;
}

const QString WizardPage_Partition_Auto::finalPartInfo()
{
    return m_automode->partitionInfo();
}


const QString WizardPage_Partition_Auto::rootPartPath()
{
    return m_automode->rootPath();
}

void WizardPage_Partition_Auto::writeConf()
{
    m_automode->writeXML();
}

bool WizardPage_Partition_Auto::autoDiskPart()
{
    qDebug() << "autoDiskPart" << endl;
    return m_automode->autoPartition();
}

const QString WizardPage_Partition_Auto::warningInfo()
{
    return QString("");
}

