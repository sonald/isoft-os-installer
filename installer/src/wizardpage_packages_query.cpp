#include "wizardpage_packages_query.h"
#include <QFile>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVariant>
#include <QVBoxLayout>

WizardPage_Packages_Query::WizardPage_Packages_Query(QWidget *parent)
    : QWizardPage(parent)
{
    m_minButton = new QRadioButton(this);
    m_minLabel = new QLabel(this);
    m_completeButton = new QRadioButton(this);
    m_completeLabel = new QLabel(this);
    m_customizeButton = new QRadioButton(this);
    m_customizeLabel = new QLabel(this);

    m_layout = new QVBoxLayout(this);

    m_minLabel->setWordWrap(true);
    m_completeLabel->setWordWrap(true);
    m_customizeLabel->setWordWrap(true);

    m_minLabel->setAlignment(Qt::AlignLeft);
    m_completeLabel->setAlignment(Qt::AlignLeft);
    m_customizeLabel->setAlignment(Qt::AlignLeft);
    const int indent =50;
    m_minLabel->setIndent(indent);
    m_completeLabel->setIndent(indent);
    m_customizeLabel->setIndent(indent);

    m_minButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_completeButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    m_customizeButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    m_minLabel->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_completeLabel->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_customizeLabel->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_minLabel->setTextFormat(Qt::RichText);
    m_completeLabel->setTextFormat(Qt::RichText);
    m_customizeLabel->setTextFormat(Qt::RichText);

    m_layout->addWidget(m_minButton);
    m_layout->addWidget(m_minLabel);
    m_layout->addWidget(m_completeButton);
    m_layout->addWidget(m_completeLabel);
    m_layout->addWidget(m_customizeButton);
    m_layout->addWidget(m_customizeLabel);

    registerField( "packageMode", this, "packageMode", SIGNAL(modeChanged()) );
    connect( m_minButton, SIGNAL( clicked() ), this, SLOT( buttonChecked() ) );
    connect( m_completeButton, SIGNAL( clicked() ), this, SLOT( buttonChecked() ) );
    connect( m_customizeButton, SIGNAL( clicked() ), this, SLOT( buttonChecked() ) );
    m_packageMode = "minimum";
    setField( "packageMode", m_packageMode );
    m_minButton->setChecked(true);
}

void WizardPage_Packages_Query::initializePage()
{
    buttonChecked();			// restore the field.
    // Since the field is created in this page, if back to page before this,
    // field will be set to default value, so should set the field according to
    // checked status.
    setTitle( tr("Packages Set") );
    setSubTitle( tr("Select the different set of packages to install.") );

    m_minButton->setText( tr("Basic") );
    m_completeButton->setText( tr("Complete") );
    m_customizeButton->setText( tr("Customize") );

    m_minLabel->setText( tr("All necessary packages to run a basic system is installed.") );

    m_completeLabel->setText( tr("All packages in install disc is installed.") );

    m_customizeLabel->setText( tr("You can customize the packages to be installed.") );
}

int WizardPage_Packages_Query::nextId() const
{
    if ( m_minButton->isChecked() )
        return Page_Summary;
    else if ( m_completeButton->isChecked() )
        return Page_Summary;
    else
        return QWizardPage::nextId();
}

void WizardPage_Packages_Query::setPackageMode(const QString& mode)
{
    if ( m_packageMode != mode ) {
        m_packageMode = mode;
        setField("packageMode", m_packageMode);
    }
}

const QString& WizardPage_Packages_Query::packageMode() const
{
    return m_packageMode;
}

void WizardPage_Packages_Query::buttonChecked() 
{
    if ( m_minButton->isChecked() )
        setPackageMode("minimum");
    else if ( m_completeButton->isChecked() )
        setPackageMode("complete");
    else
        setPackageMode("customize");
}
