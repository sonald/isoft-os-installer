#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QVBoxLayout>
#include "wizardpage_partition_simple.h"

WizardPage_Partition_Simple::WizardPage_Partition_Simple(QWidget* parent)
    : WizardPage_Partition(parent)
{
    m_simple = NULL;
    m_layout = new QVBoxLayout();
    setLayout( m_layout );
}

void WizardPage_Partition_Simple::initializePage()
{
    setTitle( tr("Partition: Simple Mode") );
    setSubTitle( tr("Select the partition on which you want to install Red Flag Linux system. You can get the OS type of partitions directly, and view the directory tree through \"detail\" button.") );

    QString locale = field("locale").toString();
    m_simple = new DisksWidget( this, DisksWidget::Simple, locale);
    m_layout->addWidget( m_simple );
    connect( m_simple, SIGNAL( currentChanged(QString, QString, QString) ),
	    this, SLOT( getCurPartInfo( QString, QString, QString) ) );
    m_canInstall = false;
}

void WizardPage_Partition_Simple::cleanupPage()
{
    if( m_simple )
	delete m_simple;
    m_canInstall = false;
}

void WizardPage_Partition_Simple::getCurPartInfo(QString dev, QString parttype, QString fstype)
{
    if ( ( parttype == "primary" ) || ( parttype == "logical" ) || ( parttype == "free" ) )
	m_canInstall = true;
    else
	m_canInstall = false;
    emit completeChanged();
}

bool WizardPage_Partition_Simple::isComplete() const
{
    return m_canInstall;
}

bool WizardPage_Partition_Simple::validatePage()
{
    QString error;
    qDebug() << "validate simple" << endl;
    if ( !m_simple->validate(error) ) {
	QMessageBox::warning(this, tr("Partition: Simple Mode"), error );
	return false;
    } else {
	m_simple->installOnPartition();
	return true;
    }
}

const QString WizardPage_Partition_Simple::finalPartInfo()
{
    return m_simple->finalPartitionsInfo();
}

const QString WizardPage_Partition_Simple::warningInfo()
{
    return m_simple->warningInfo();
}

const QString WizardPage_Partition_Simple::rootPartPath()
{
    m_simple->doSimpleInstall();
    return m_simple->rootPartitionPath();
}

void WizardPage_Partition_Simple::writeConf()
{
    return m_simple->writeXML();
}
