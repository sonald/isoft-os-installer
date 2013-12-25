#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QVBoxLayout>
#include "wizardpage_partition_advanced.h"

WizardPage_Partition_Advanced::WizardPage_Partition_Advanced(QWidget *parent)
    : WizardPage_Partition(parent)
{
    m_advanced = NULL;
    m_layout = new QVBoxLayout();
    setLayout( m_layout );
}

void WizardPage_Partition_Advanced::initializePage()
{
    setTitle( tr("Partition: Advanced Mode") );
    setSubTitle( tr("You can create, delete, edit partition or view the directory tree of partition, set the mount point, file system and whether formatted.") );
    
    QString locale = field("locale").toString();
    m_advanced = new DisksWidget( this, DisksWidget::Advanced, locale );
    m_layout->addWidget( m_advanced );
}

void WizardPage_Partition_Advanced::cleanupPage()
{
    if( m_advanced )
	delete m_advanced;
}

bool WizardPage_Partition_Advanced::validatePage()
{
    QString error;
    bool ret = m_advanced->validate(error);
    if ( !ret )
	QMessageBox::warning(this, tr("Partition: Advanced Mode"), error );
    return ret;
}

const QString WizardPage_Partition_Advanced::finalPartInfo()
{
    return m_advanced->finalPartitionsInfo();
}

const QString WizardPage_Partition_Advanced::warningInfo()
{
    return m_advanced->warningInfo();
}

const QString WizardPage_Partition_Advanced::rootPartPath()
{
    return m_advanced->rootPartitionPath();
}

void WizardPage_Partition_Advanced::writeConf()
{
    m_advanced->writeXML();
}

