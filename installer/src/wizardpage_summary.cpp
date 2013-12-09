#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <cassert>
#include <installengine.h>
#include "wizardpage_partition.h"
#include "wizardpage_summary.h"
#include "dialog_summary.h"
#include "parser_locale.h"

WizardPage_Summary::WizardPage_Summary(QWidget *parent)
    : QWizardPage(parent)
{
    // GUI 
    m_summary = new QTextEdit(this);
    m_summary->setReadOnly(true);

    m_spacerItem = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);
    //m_advanced = new QPushButton( this );
    //m_buttonLayout = new QHBoxLayout();
    //m_buttonLayout->addItem( m_spacerItem );
    //m_buttonLayout->addWidget( m_advanced, 0, Qt::AlignRight );

    m_warning = new QLabel(this);
    m_warning->setWordWrap(true);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_summary);
    //m_layout->addLayout(m_buttonLayout);
    m_layout->addWidget(m_warning);
    setLayout(m_layout);

    // self-doc
    setCommitPage(true);

    registerField( "pathGrub", this, "pathGrub", SIGNAL(pathGrubChanged()) );

    //connect( m_advanced, SIGNAL( clicked() ), this, SLOT( advancedDialog() ) );
}

void WizardPage_Summary::initializePage()
{
    // set the text of GUI
    setTitle( tr("Pre-Install Summary") );
    setSubTitle( tr("Please read the pre-install summary carefully before starting installation progress." ) );

    //m_advanced->setText( tr("Advanced") );
    m_warning->setText( tr( "Notice: Please confirm the pre-install summary. You can't go back to previous page after this step. You still can back to the previous wizard page for re-setup now.") );

    // init the flags about grub install
    m_instBootLoader = true;
    m_mbr = true;

    // fill the contents of summary: Version, Lang, PartitionInfo.
    m_summary->clear();

    // Version
    m_summary->append( tr("Red Flag Linux For Loongson") );
    m_summary->append( "" );

    // Lang
    QString lineLang;
    lineLang += tr("Language: ");

    QString locale = field("locale").toString();
    ParserLocale parser;
    bool ret = parser.open( g_localexml );
    if ( !ret ) 
	qDebug() << "rflocale.xml is broken.";
    assert( ret );
    
    QString langName = parser.nameWithLocale( locale );
    lineLang += langName;

    m_summary->append( lineLang );
    m_summary->append( "" );

    // Part
    // get the mode and calculate the page.
    /*
    m_mode = field("partitionMode").toString();
    enum Page_ID pageId;
    if ( m_mode == "auto" )
	pageId = Page_Partition_Auto;
    else if ( m_mode == "simple" )
	pageId = Page_Partition_Simple;
    else if ( m_mode == "advanced" )
	pageId = Page_Partition_Advanced;
    else
	pageId = Page_Error;
    assert( pageId != Page_Error );
    */

    m_page = (WizardPage_Partition*) wizard()->page(Page_Partition_Advanced);

    m_summary->append( tr("Partition Information:") );
    m_summary->append( m_page->finalPartInfo() );
    m_summary->append( "" );

    // set the init value of field "pathGrub".
    m_pathGrub = QString();
    setField("pathGrub", m_pathGrub );
}

int WizardPage_Summary::nextId() const
{
    return Page_Progress;
}

bool WizardPage_Summary::validatePage()
{
    // confirm before installation.
    if ( ! m_page->warningInfo().isEmpty() ) {
	QMessageBox::StandardButton buttonClicked = QMessageBox::information(this, 
		tr("Installation Information"), m_page->warningInfo(), 
		QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok );
	
	if ( buttonClicked == QMessageBox::Cancel )
	    return false;
    }
    // Ok, user confirm it. write installation instructions into xml.
    // Get the grub info.
    QString path = m_page->rootPartPath();
    qDebug()<<"root device path is:" << path;
    setField( "pathGrub", path );

    m_page->writeConf();
    return true;
}

void WizardPage_Summary::setPathGrub(const QString& path)
{
    if ( m_pathGrub !=  path ) {
	m_pathGrub = path;
	emit pathGrubChanged();
    }
}

const QString& WizardPage_Summary::pathGrub() const
{
    return m_pathGrub;
}

void WizardPage_Summary::advancedDialog()
{
    DialogSummary dialog( this, m_instBootLoader, m_mbr );

    int ret = dialog.exec();
    if ( ret == QDialog::Accepted ) {
	m_instBootLoader = dialog.isInstall();
	m_mbr = dialog.isMBR();
    }
}
