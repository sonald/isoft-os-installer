#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include "dialog_reject.h"

DialogReject::DialogReject(QWidget* parent)
    : QDialog(parent)
{
    m_warning = new QLabel(this);
    m_warning->setWordWrap( true );
    m_quest = new QLabel(this);
    
    m_spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    m_ok = new QPushButton( tr("&Yes"), this );
    m_cancel = new QPushButton( tr("&No"), this );
    m_layoutButton = new QHBoxLayout();
    m_layoutButton->addItem( m_spacer );
    m_layoutButton->addWidget( m_ok );
    m_layoutButton->addWidget( m_cancel );
    
    m_layoutForm = new QVBoxLayout();
    m_layoutForm->addWidget(m_warning);
    m_layoutForm->addWidget(m_quest);
    m_layoutForm->addLayout(m_layoutButton);
    setLayout( m_layoutForm );    

    m_quest->setText( tr("Do you want to quit?") );
    setWindowTitle( tr("Quit the installer?") );

    connect( m_ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void DialogReject::setWarning( const QString& warn)
{
    m_warning->setText( warn );
}
