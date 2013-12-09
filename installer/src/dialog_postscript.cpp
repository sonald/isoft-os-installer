#include <QCloseEvent>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QPalette>
#include "installer_global.h"
#include "dialog_postscript.h"

DialogPostscript::DialogPostscript(QWidget* parent)
    : QDialog(parent)
{
    m_layout = new QHBoxLayout();

    m_gif = new QMovie( g_appImgPath + "/throbber.gif" );
    if ( m_gif->isValid() ) {
	QPalette pal = palette();
	QColor color = pal.color(QPalette::Window);
	m_gif->setBackgroundColor(color);
	m_labelGif = new QLabel( this );
	m_labelGif->setMovie( m_gif );
	m_layout->addWidget( m_labelGif );
    } else {
	delete m_gif;
	m_gif = NULL;
	m_labelGif = NULL;
    }
    
    m_labelText = new QLabel( this );
    m_layout->addWidget( m_labelText );
    setLayout( m_layout );
}

DialogPostscript::~DialogPostscript()
{
    if ( m_gif )
	delete m_gif;
}

void DialogPostscript::setLabelText()
{
    setWindowTitle( tr("PostInstall") );
    m_labelText->setText( tr("Configuring the system now. Please waiting...") );
    if ( m_labelGif )
	m_gif->start();	
}

void DialogPostscript::closeEvent(QCloseEvent* event)
{
    event->ignore();
}
