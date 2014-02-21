#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTranslator>
#include <QVariant>
#include <QApplication>

#include <cassert>

#include "wizardpage_welcome.h"
#include "installer_global.h"
#include "archinfo.h"

WizardPage_Welcome::WizardPage_Welcome(QWidget *parent)
    : QWizardPage(parent)
{
    // create the UI elements.
    // listwidget for language.
    m_listWidget = new QListWidget(this);

    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setLayoutMode(QListView::Batched);
    m_listWidget->setBatchSize(50);
    m_listWidget->setFlow(QListView::TopToBottom);
    m_listWidget->setWrapping(false);
    m_listWidget->setWordWrap(false);

    // label for welcome sentence.
    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignTop);
    
    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_listWidget);
    m_layout->addWidget(m_label);
    m_layout->setStretchFactor(m_listWidget, 1);
    m_layout->setStretchFactor(m_label, 2);
    setLayout(m_layout);

    // register field "locale".
    m_locale = QString();
    registerField( "locale", this, "locale", SIGNAL( localeChanged() ) );
    
    bool ret = m_localeParser.open( g_localexml );
    if ( !ret ) 
	qDebug() << "locale.xml is not existed or wrong formatted.";
    assert ( ret );

    connect( m_listWidget, SIGNAL( currentTextChanged( const QString & ) ), 
	    	this, SLOT( updateLocale( const QString & ) ) );
    connect( this, SIGNAL( localeChanged() ), this, SLOT( reTranslateUi() ) );
}

void WizardPage_Welcome::reTranslateUi()
{
    ArchInfo ai;
    qDebug() << ai.os();
    setTitle( tr("Welcome") );
    setSubTitle( tr("Welcome to %1. You can select the language of installer now.").arg(ai.os()) );
    m_label->setText( trUtf8("%1 support Simplified-Chinese, English. "
                "You can select the language which will be the language of installer. And the selected "
                "language will be the default language of installed system.").arg(ai.os()) );
}

void WizardPage_Welcome::initializePage()
{
    qDebug() << field("selectedGroups").toStringList();
    QString localeDefault = m_localeParser.localeMatched( QLocale::system() );
    QList<QString> list = m_localeParser.listLocaleEnabled();

    for ( QList<QString>::const_iterator itor = list.begin();
	  itor != list.end(); itor++ )
	{
	    QString locale = (*itor);
	    QString name = m_localeParser.nameWithLocale( locale );
	    QListWidgetItem* cur = new QListWidgetItem( name , m_listWidget );
	    if ( locale == localeDefault )
		m_listWidget->setCurrentItem( cur );
	}
 
    qDebug() << localeDefault;
    setField("locale", localeDefault);
    g_appTranslator.load( g_transPrefix + "_" + localeDefault, g_appQmPath );
    g_qtTranslator.load( "qt_" + localeDefault, g_qtQmPath );

    reTranslateUi(); // set the text of UI.
}

int WizardPage_Welcome::nextId() const
{
    return QWizardPage::nextId();
}

// Load the new translate files, update the field "locale".
// Loading the translate files here, because localeChanged() signal
// will be sent to wizard and wizardpage both.
void WizardPage_Welcome::updateLocale(const QString & currentText)
{
    QString locale = m_localeParser.localeWithName( currentText );
    g_appTranslator.load( g_transPrefix + "_" + locale, g_appQmPath );
    g_qtTranslator.load( "qt_" + locale, g_qtQmPath );
    qApp->installTranslator(&g_appTranslator);
    qApp->installTranslator(&g_qtTranslator);
    setField("locale", locale);
    setlocale(LC_MESSAGES, locale.toUtf8().constData());
}

// Get the value of property "locale"
const QString& WizardPage_Welcome::locale() const
{
    return m_locale;
}

// Set the value of property "locale"
void WizardPage_Welcome::setLocale(const QString& locale)
{
    if( m_locale != locale ) {
	m_locale = locale;
	emit localeChanged();
    }
}
