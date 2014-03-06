#include <QCloseEvent>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "dialog_summary.h"

DialogSummary::DialogSummary(QWidget* parent, bool install, bool mbr)
    :QDialog(parent)
{
    m_mbr = new QRadioButton(this);
    m_root = new QRadioButton(this);
    m_groupLayout = new QVBoxLayout();
    m_groupLayout->addWidget(m_mbr);
    m_groupLayout->addWidget(m_root);

    m_groupBox = new QGroupBox(this);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(true);
    m_groupBox->setLayout(m_groupLayout);

    m_spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_ok = new QPushButton(this);
    m_cancel = new QPushButton(this);

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addItem( m_spacerItem );
    m_buttonLayout->addWidget( m_ok );
    m_buttonLayout->addWidget( m_cancel);

    m_formLayout = new QVBoxLayout();
    m_formLayout->addWidget(m_groupBox);
    m_formLayout->addLayout(m_buttonLayout);
    setLayout(m_formLayout);

    m_mbr->setText( tr("install to Main Boot Record. (Recommended)" ) );
    m_root->setText( tr("install to Root Partition." ) );
    m_groupBox->setTitle( tr("Install Boot Loader") );
    m_ok->setText( tr( "&OK" ) );
    m_cancel->setText( tr( "&Cancel" ) );
    setWindowTitle( tr("Advanced Setting") );

    m_flagInstall = install;
    m_flagMBR = mbr;
    m_groupBox->setChecked( m_flagInstall );
    if ( m_flagMBR )
	m_mbr->setChecked( true );
    else
	m_root->setChecked( true );

    connect( m_ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

bool DialogSummary::isMBR()
{
    return m_flagMBR;
}

bool DialogSummary::isInstall()
{
    return m_flagInstall;
}

void DialogSummary::setMBR(bool mbr)
{
    m_flagMBR = mbr;
    if ( mbr )
	m_mbr->setChecked( true );
    else
	m_root->setChecked( true );
}

void DialogSummary::setInstall(bool install)
{
    m_flagInstall = install;
    m_groupBox->setChecked( install );
}

void DialogSummary::accept()
{
    if ( m_groupBox->isChecked() )
	m_flagInstall = true;
    else
	m_flagInstall = false;

    if ( m_mbr->isChecked() )  // mbr
	m_flagMBR = true;
    else			// root
	m_flagMBR = false;

    QDialog::accept();
}
