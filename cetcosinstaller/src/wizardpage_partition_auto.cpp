#include <QDebug>
#include "partition_automode.h"
#include "wizardpage_partition_auto.h"


WizardPage_Partition_Auto::WizardPage_Partition_Auto(QWidget* parent)
    : WizardPage_Partition(parent)
{
    m_automode = new PartitionAutoMode;

    m_sigMap = new QSignalMapper(this);
    connect(m_sigMap, SIGNAL(mapped(QString)), this, SLOT(changeDestinationDisk(QString)));

    m_layout = new QVBoxLayout;
    PartedDevices disks;
    for (int i = 0; i < disks.count(); ++i) {
        addDisk(disks.device(i));
    }
    setLayout(m_layout);
}

void WizardPage_Partition_Auto::addDisk(Device *disk)
{
    QRadioButton *rb = new QRadioButton;
    rb->setText(disk->path());
    connect(rb, SIGNAL(clicked()), m_sigMap, SLOT(map()));
    m_sigMap->setMapping(rb, QString(disk->path()));
    m_layout->addWidget(rb);
}

void WizardPage_Partition_Auto::changeDestinationDisk(const QString &disk)
{
    qDebug() << "auto part disk " << disk;
    m_automode->autoPartFulldisk(disk);
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

