#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QRegExp>
#include <QSpacerItem>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>
#include <parted++/parted++.h>
#include <cassert>
#include "wizardpage_partition_auto.h"
#include "wizardpage_partition_mode.h"
#include <assert.h>

WizardPage_Partition_Mode::WizardPage_Partition_Mode(QWidget *parent)
    : QWizardPage(parent)
{
    // set the GUI elements.
    m_autoButton = new QRadioButton(this);
    m_autoLabel = new QLabel(this);
    m_simpleButton = new QRadioButton(this);
    m_simpleLabel = new QLabel(this);
    m_advancedButton = new QRadioButton(this);
    m_advancedLabel = new QLabel(this);
    
    m_layout = new QVBoxLayout(this);
    
    m_autoLabel->setWordWrap(true);
    m_simpleLabel->setWordWrap(true);
    m_advancedLabel->setWordWrap(true);

    m_autoLabel->setAlignment(Qt::AlignLeft);
    m_simpleLabel->setAlignment(Qt::AlignLeft);
    m_advancedLabel->setAlignment(Qt::AlignLeft);
    const int indent =50;
    m_autoLabel->setIndent(indent);
    m_simpleLabel->setIndent(indent);
    m_advancedLabel->setIndent(indent);

    m_autoButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_simpleButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_advancedButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    m_autoLabel->setTextFormat(Qt::RichText);
    m_simpleLabel->setTextFormat(Qt::RichText);
    m_advancedLabel->setTextFormat(Qt::RichText);

    m_layout->addWidget(m_autoButton);
    m_layout->addWidget(m_autoLabel);
    m_layout->addWidget(m_simpleButton);
    m_layout->addWidget(m_simpleLabel);
    m_layout->addWidget(m_advancedButton);
    m_layout->addWidget(m_advancedLabel);
    
    registerField( "partitionMode", this, "partitionMode", SIGNAL(modeChanged()) );
    
    // disable the auto mode default.
    m_autoLabel->setEnabled( false );
    m_autoButton->setEnabled( false );
    m_simpleLabel->setEnabled(false);
    m_simpleButton->setEnabled(false);
}

void WizardPage_Partition_Mode::initializePage()
{
    m_advancedButton->setChecked(true);
    m_partitionMode = "simple";
    setField( "partitionMode", m_partitionMode );

    setTitle( tr("Partition Mode") );
    setSubTitle( tr("Red Flag Linux Installer support three partition mode: auto, simple and advanced.") );

    m_autoButton->setText( tr("Auto Mode") );
    m_simpleButton->setText( tr("Simple Mode") );
    m_advancedButton->setText( tr("Advanced Mode") );

    m_autoLabel->setText( tr("In this mode, a free partition will be used to install, swap partition will be create if necessary. Only appropriate size of space will be used if the partition is too big. Any existed partition will not be affected. You can check the result in summary page.") );
    
    m_simpleLabel->setText( tr("This mode give you a quick and simple way to install. Just select the partition on which you want to install, then press \"next\" button, all is done." ) );
    
    m_advancedLabel->setText( tr("In this mode, you can partition the disk manually. You can view the contents of partition, add and delete partition, edit partition to change the mountpoint, file system and format.") );

    bool canAuto;
    canAuto = ((WizardPage_Partition_Auto*)wizard()->page( Page_Partition_Auto ))-> autoDiskPart();
    if ( canAuto ) {
	m_autoLabel->setEnabled( true );
	m_autoButton->setEnabled( true );
    }
    
    QTimer::singleShot(200, this, SLOT( checkHDInstall() ) );
}

void WizardPage_Partition_Mode::checkHDInstall()
{
    QFile cmdlineFile("/proc/cmdline");
    cmdlineFile.open( QIODevice::ReadOnly | QIODevice::Text );
    QTextStream cmdlineStream( &cmdlineFile );
    QString line = cmdlineStream.readLine();
    cmdlineFile.close();
    if ( line.contains("isodev") ) {
	QStringList lineList = line.split(QRegExp("\\s+"));
	QStringList listOnlyIsodev = lineList.filter("isodev=");
	assert( listOnlyIsodev.size() == 1 );
	QString isodevStr = listOnlyIsodev.at(0);
	QStringList isodevList = isodevStr.split("=");
	assert( isodevList.size() == 2 );
	QString devpath = isodevList.at(1);
	QMessageBox::information(this, tr("LiveCD hard disk mode"), 
				 QString( tr("You has launched the LiveCD with hard disk mode. \nThe partition(%1) on which LiveCD iso file put must not be used.")).arg(devpath) );
    }
}

int WizardPage_Partition_Mode::nextId() const
{
    if( m_autoButton->isChecked() )
	return Page_Summary;
    else if( m_simpleButton->isChecked() )
	return Page_Partition_Simple;
    else 
	return Page_Partition_Advanced;
}

void WizardPage_Partition_Mode::setPartitionMode(const QString& mode)
{
    if ( m_partitionMode != mode ) {
	m_partitionMode = mode;
	emit modeChanged();
    }
}

const QString& WizardPage_Partition_Mode::partitionMode() const
{
    return m_partitionMode;
}

bool WizardPage_Partition_Mode::validatePage()
{
    qDebug() << "Mode ValidatePage.";

    if ( m_autoButton->isChecked() )
	setPartitionMode("auto");
    else if ( m_simpleButton->isChecked() )
	setPartitionMode("simple");
    else 
	setPartitionMode("advanced");
    
    return true;
}
