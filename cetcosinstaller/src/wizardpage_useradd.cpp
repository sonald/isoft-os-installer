#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSpacerItem>
#include <QStringList>
#include <QTextStream>
#include <QVBoxLayout>
#include <QVariant>
#include <installengine.h>
#include "wizardpage_useradd.h"

const int PASSWORD_LENGTH =128;
const int USERNAME_LENGTH =32;	// this magic number is relative to KDM. 

QValidator::State NameValidator::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos)
    int size = input.size();
    int index = 0;

    // limit the length of username.
    if ( input.toUtf8().size() >= USERNAME_LENGTH )
        return QValidator::Invalid;

    // not begin with -
    if ( input.at(0) == 0x002D )
        return QValidator::Invalid;

    // validate the username.
    while( index < size ) {
        QChar ch = input.at(index);
        if ( !isValid(ch) )
            return QValidator::Invalid;
        index++;
    }
    return QValidator::Acceptable;
}

/*
 * numbers, english characters, 3 characters and CJK Common Ideographs.
 * CJK Common is more than Chinese Ideographs, but it's ok.
 */
bool NameValidator::isValid(const QChar& ch) const
{
    if( ch >= 0x0030 && ch <= 0x0039 ) // 0-9
        return true;
    if( ch >= 0x0041 && ch <= 0x005A ) // A-Z
        return true;
    if( ch >= 0x0061 && ch <= 0x007A ) // a-z
        return true;
    if( ch == 0x002D || ch == 0x002E || ch == 0x005F ) // - . _
        return true;
    if( ch >= 0x4E00 && ch <= 0x9FFF ) // CJK Unified Ideographs: Common
        return true;
    return false;
}


WizardPage_UserAdd::WizardPage_UserAdd(QWidget *parent)
    : QWizardPage(parent)
{
    // Construct the GUI elements, set the validators.
    m_rootPicture = new QLabel(this);
    QPixmap rootPicture( g_appImgPath + "/root.png");
    m_rootPicture->setPixmap(rootPicture);
    m_rootPicture->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    m_rootDescript = new QLabel(this);
    m_descriptLayout = new QHBoxLayout();
    m_descriptLayout->addWidget( m_rootPicture );
    m_descriptLayout->addWidget( m_rootDescript );

    m_passwd = new QLineEdit(this);
    m_passwd->setDragEnabled(false);
    m_passwd->setEchoMode(QLineEdit::Password);
    m_passwd->setMaxLength(PASSWORD_LENGTH);

    m_confirm = new QLineEdit(this);
    m_confirm->setDragEnabled(false);
    m_confirm->setEchoMode(QLineEdit::Password);
    m_confirm->setMaxLength(PASSWORD_LENGTH);

    QRegExp passwdSyntax("[a-zA-Z0-9]*");
    QValidator* passwdValidator = new QRegExpValidator(passwdSyntax, this);
    m_passwd->setValidator( passwdValidator );
    m_confirm->setValidator( passwdValidator );

    m_passwdLabel = new QLabel(this);
    m_confirmLabel = new QLabel(this);

    m_passwdLayout = new QGridLayout();
    m_passwdLayout->addWidget( m_passwdLabel, 0,0,1,1 );
    m_passwdLayout->addWidget( m_passwd, 0,1,1,1 );
    m_passwdLayout->addWidget( m_confirmLabel, 1,0,1,1 );
    m_passwdLayout->addWidget( m_confirm, 1,1,1,1 );

    m_rootLayout = new QVBoxLayout();
    m_rootLayout->addLayout( m_descriptLayout );
    m_rootLayout->addLayout( m_passwdLayout );

    m_userPicture = new QLabel(this);
    QPixmap userPicture( g_appImgPath + "/user.png");
    m_userPicture->setPixmap(userPicture);
    m_userPicture->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_userLabel = new QLabel(this);
    m_labelLayout = new QHBoxLayout();
    m_labelLayout->addWidget(m_userPicture);
    m_labelLayout->addWidget(m_userLabel);

    m_user1 = new QLineEdit(this);
    m_user2 = new QLineEdit(this);
    m_user3 = new QLineEdit(this);
    m_user1->setMaxLength(USERNAME_LENGTH);
    m_user2->setMaxLength(USERNAME_LENGTH);
    m_user3->setMaxLength(USERNAME_LENGTH);

    NameValidator* nameValidator = new NameValidator(this);
    m_user1->setValidator( nameValidator );
    m_user2->setValidator( nameValidator );
    m_user3->setValidator( nameValidator );

    m_warningLabel = new QLabel(this);

    m_userLayout = new QGridLayout();
    m_userLayout->addLayout(m_labelLayout, 0, 0, 1, 2);
    m_userLayout->addWidget(m_user1, 1, 0);
    m_star = new QLabel("*");
    m_userLayout->addWidget(m_star, 1, 1);
    m_userLayout->addWidget(m_user2, 2, 0);
    m_userLayout->addWidget(m_user3, 3, 0);
    m_userLayout->addWidget(m_warningLabel, 4, 0, 1, 2);

    m_spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    m_formLayout = new QVBoxLayout(this);
    m_formLayout->addLayout( m_rootLayout );
    m_formLayout->addItem( m_spacer );
    m_formLayout->addLayout( m_userLayout );

    setLayout(m_formLayout);

    connect( m_user1, SIGNAL( textChanged(const QString&) ), this, SLOT( checkUser1(const QString&) ) );
    connect( m_user2, SIGNAL( textChanged(const QString&) ), this, SLOT( checkUser2(const QString&) ) );
    connect( m_user3, SIGNAL( textChanged(const QString&) ), this, SLOT( checkUser3(const QString&) ) );
    connect( m_user1, SIGNAL( editingFinished() ), this, SLOT( showWarningLabel() ) );
    connect( m_user2, SIGNAL( editingFinished() ), this, SLOT( showWarningLabel() ) );
    connect( m_user3, SIGNAL( editingFinished() ), this, SLOT( showWarningLabel() ) );

    // init the stringlist of names in /etc/passwd.
    m_listSysUsers = QStringList();
}

void WizardPage_UserAdd::initializePage()
{
    // set the text.
    m_passwd->setText( QString("") );
    m_confirm->setText( QString("") );

    setTitle( tr("Set password and add users") );
    setSubTitle( tr("Password for user \"root\" must be set. You can add new user also, at least one new user account should be created.") );

    m_rootDescript->setText( tr("User \"root\" can control everything on the entire computer.\nA password MUST be set for user \"root\".") );
    m_passwdLabel->setText( tr("Password:" ) );
    m_confirmLabel->setText( tr("Confirm:" ) );

    m_userLabel->setText( tr("Add new users:") );
    m_warningLabel->setText( "" );
    m_warningLabel->hide();

    // read the /etc/passwd, fulfill the list.
    readSysUsers(&m_listSysUsers);

    // m_legal: 0 all ok; 1, user1; 2, user2; 4 user3; 3, user1 and user2.
    m_illegal = 0;
    m_dual = false;
}

int WizardPage_UserAdd::nextId() const
{
    return Page_Finish;
}

bool WizardPage_UserAdd::validatePage()
{
    if (m_passwd->text().isEmpty()) {
        QMessageBox::warning(this, tr("Root password needed"), tr("Root password should not be empty."));
        return false;
    }

    bool passwdSame = ( m_passwd->text() == m_confirm->text() );
    if ( !passwdSame ) {
        QMessageBox::warning(this, tr("Root password"), tr("Please input password correctly twice.") );
        m_passwd->setText( QString("") );
        m_confirm->setText( QString("") );
        return false;
    }

    if (m_user1->text().isEmpty()) {
        QMessageBox::warning(this, tr("User"), tr("Please at least add one normal user.") );
        return false;
    }

    return true;
}

void WizardPage_UserAdd::setPasswdUser()
{
    g_engine->cmdSetRootPassword( m_passwd->text().toLatin1() );
    if (!m_user1->text().isEmpty()) g_engine->cmdAddUser( m_user1->text().toUtf8() );
    if (!m_user2->text().isEmpty()) g_engine->cmdAddUser(m_user2->text().toUtf8() );
    if (!m_user3->text().isEmpty()) g_engine->cmdAddUser(m_user3->text().toUtf8() );
}

bool WizardPage_UserAdd::isComplete() const
{
    if ( !m_illegal && !m_dual )
        return true;
    else
        return false;
}

void WizardPage_UserAdd::showWarningLabel()
{
    QString warning = tr("");
    if ( m_dual ) {
        warning += tr("Please not fill the dulplicate name.\n");
        if ( m_user1->text() == m_user2->text()
             || m_user1->text() == m_user3->text() )
            m_illegal = m_illegal & 0xFFFE ;
        if ( m_user2->text() == m_user3->text() )
            m_illegal = m_illegal & 0xFFFD ;
    }

    QString name = tr("");
    if ( m_illegal & 0x0001 ) {
        name += m_user1->text();
        name += " ";
    }
    if ( m_illegal & 0x0002 ) {
        name += m_user2->text();
        name += " ";
    }
    if ( m_illegal & 0x0004 ) {
        name += m_user3->text();
        name += " ";
    }
    if ( !name.isEmpty() ) {
        name = " " + name;
        warning += QString( tr("The username ( %1 ) had existed.") ).arg(name);
    }
    if ( !warning.isEmpty() ) {
        m_warningLabel->setText(warning);
        m_warningLabel->show();
    }
}

void WizardPage_UserAdd::checkUser1(const QString& name)
{
    m_warningLabel->hide();

    m_illegal = m_illegal & 0xFFFE ;

    if ( m_listSysUsers.contains( name ) ) {
        m_illegal = m_illegal | 0x0001 ;
    }
    m_dual = checkUserNameDual();

    emit completeChanged();
}

void WizardPage_UserAdd::checkUser2(const QString& name)
{
    m_warningLabel->hide();

    m_illegal = m_illegal & 0xFFFD ;
    m_dual = m_dual & 0xFFFD ;

    if ( m_listSysUsers.contains( name ) ) {
        m_illegal = m_illegal | 0x0002 ;
    }
    m_dual = checkUserNameDual();

    emit completeChanged();
}

void WizardPage_UserAdd::checkUser3(const QString& name)
{
    m_warningLabel->hide();

    m_illegal = m_illegal & 0xFFFB ;
    m_dual = m_dual & 0xFFFB ;

    if ( m_listSysUsers.contains( name ) ) {
        m_illegal = m_illegal | 0x0004 ;
    }
    m_dual = checkUserNameDual();

    emit completeChanged();
}

bool WizardPage_UserAdd::checkUserNameDual()
{
    bool ret = false;

    if ( m_user1->text() == m_user2->text() || m_user1->text() == m_user3->text() )
        if ( !m_user1->text().isEmpty() )
            ret = true;
    if ( m_user2->text() == m_user3->text() )
        if ( !m_user2->text().isEmpty() )
            ret = true;
    return ret;
}

void WizardPage_UserAdd::readSysUsers(QStringList* list)
{
    QFile filePasswd("/etc/passwd");
    if (!filePasswd.open( QIODevice::ReadOnly | QIODevice::Text ))
        return;

    QTextStream inPasswd( &filePasswd );
    while( !inPasswd.atEnd() ) {
        QString line = inPasswd.readLine();
        line.truncate( line.indexOf(':') );
        *list << line;
    }
    filePasswd.close();
}