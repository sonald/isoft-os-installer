#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "dialog_root.h"

DialogRoot::DialogRoot(QWidget *parent) 
    : QDialog(parent)
{
    setWindowTitle( tr("Installer") );
    m_label = new QLabel( tr("Root permission is need to start installer.") );
    m_button = new QPushButton( tr("Close") );
    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_button);
    setLayout(m_layout);
    connect(m_button, SIGNAL( clicked() ), this, SLOT( accept()) );
}
