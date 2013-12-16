#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QTextBrowser>
#include "wizardpage_licence.h"

WizardPage_Licence::WizardPage_Licence(QWidget *parent)
    : QWizardPage(parent)
{

    m_layout = new QGridLayout(this);
    m_browserLicence = new QTextBrowser(this);
    m_browserLicence->setOpenExternalLinks(true);
    m_layout->addWidget(m_browserLicence, 0, 0, 1, 3);

    m_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addItem(m_spacer, 1, 0, 1, 1);
    
    m_radiobuttonDecline = new QRadioButton(this);
    m_radiobuttonAccept = new QRadioButton(this);
    
    m_layout->addWidget(m_radiobuttonAccept, 1, 1, 1, 1);
    m_layout->addWidget(m_radiobuttonDecline, 1, 2, 1, 1);

    registerField("Accept*", m_radiobuttonAccept);

    QStringList searchPath;
    searchPath += g_appQmPath;
    m_browserLicence->setSearchPaths(searchPath);
}

void WizardPage_Licence::initializePage()
{
    setTitle( tr("Licence Declaration") );
    setSubTitle( tr("Please read Red Flag Linux Licence.") );
    m_radiobuttonDecline->setText( tr("Decline Red Flag Linux Licence") );
    m_radiobuttonAccept->setText( tr("Accept Red Flag Linux Licence") );

    QString locale = field("locale").toString();
    QString licenceFilename = "licence_" + locale + ".html" ;
    m_browserLicence->setSource( licenceFilename );
}
