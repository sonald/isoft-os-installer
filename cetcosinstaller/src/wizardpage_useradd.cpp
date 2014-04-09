#include <QtCore>
#include <QtGui>
#include <installengine.h>
#include "wizardpage_useradd.h"

const int PASSWORD_LENGTH = 128;
const int USERNAME_LENGTH = 32;  // this magic number is relative to KDM. 

QValidator::State NameValidator::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos)
    int size = input.size();
    int index = 0;

    // limit the length of username.
    if ( input.toUtf8().size() > USERNAME_LENGTH )
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

//validation rule according to `man useradd`
QValidator::State StrictNameValidator::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos)
    if (input.length() == 0) // check empty user name at confirm stage
        return QValidator::Acceptable;

    // limit the length of username.
    if (input.toUtf8().size() >= USERNAME_LENGTH)
        return QValidator::Invalid;

    QRegExp reName("[a-z_][a-z0-9_]*[$]?");
    if (reName.exactMatch(input))
        return QValidator::Acceptable;

    return QValidator::Invalid;
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

    m_passwdLayout = new QGridLayout();
    m_passwdLayout->addWidget( new QLabel(tr("Password:")), 0,0,1,1 );
    //m_passwdLayout->addWidget( new QLabel(tr("Password:")), 0,0,1,1 );
    m_passwdLayout->addWidget( m_passwd, 0,1,1,1 );
    m_passwdLayout->addWidget( new QLabel("*"), 0,2 );
    m_passwdLayout->addWidget( new QLabel(tr("Confirm:")), 1,0,1,1 );
    m_passwdLayout->addWidget( m_confirm, 1,1,1,1 );
    m_passwdLayout->addWidget( new QLabel("*"), 1,2 );

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
    m_user1->setMaxLength(USERNAME_LENGTH);

    m_userPasswd = new QLineEdit(this);
    m_userPasswd->setDragEnabled(false);
    m_userPasswd->setEchoMode(QLineEdit::Password);
    m_userPasswd->setMaxLength(PASSWORD_LENGTH);

    m_userConfirm = new QLineEdit(this);
    m_userConfirm->setDragEnabled(false);
    m_userConfirm->setEchoMode(QLineEdit::Password);
    m_userConfirm->setMaxLength(PASSWORD_LENGTH);

    QValidator* nameValidator = new StrictNameValidator(this);
    m_user1->setValidator( nameValidator );

    m_warningLabel = new QLabel(this);

    m_userLayout = new QGridLayout();
    m_userLayout->addLayout(m_labelLayout, 0, 0, 1, 2);
    m_userLayout->addWidget(new QLabel(tr("Username:")), 1, 0);
    m_userLayout->addWidget(m_user1, 1, 1);
    m_userLayout->addWidget( new QLabel("*"), 1, 2 );
    m_userLayout->addWidget(new QLabel(tr("Password:")), 2, 0, Qt::AlignRight);
    m_userLayout->addWidget(m_userPasswd, 2, 1);
    m_userLayout->addWidget(new QLabel(tr("Confirm:")), 3, 0, Qt::AlignRight);
    m_userLayout->addWidget(m_userConfirm, 3, 1);
    m_userLayout->addWidget(m_warningLabel, 4, 0, 1, 2);

    m_spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    m_formLayout = new QVBoxLayout(this);
    m_formLayout->addLayout( m_rootLayout );
    m_formLayout->addItem( m_spacer );
    m_formLayout->addLayout( m_userLayout );

    setLayout(m_formLayout);

    connect( m_user1, SIGNAL( textChanged(const QString&) ), this, SLOT( checkUser1(const QString&) ) );
    connect( m_user1, SIGNAL( editingFinished() ), this, SLOT( showWarningLabel() ) );

    // init the stringlist of names in /etc/passwd.
    m_listSysUsers = QStringList();
}

void WizardPage_UserAdd::initializePage()
{
    wizard()->button( QWizard::CancelButton )->setEnabled( false );
    // set the text.
    m_passwd->setText( QString("") );
    m_confirm->setText( QString("") );

    setTitle( tr("Set password and add normal user") );
    setSubTitle( tr("Password for user \"root\" must be set and you should add normal user for daily usage.") );

    m_rootDescript->setText( tr("User \"root\" can control everything on the entire computer.\nA set password for \"root\" here.") );

    m_userLabel->setText( tr("Add new users:") );
    m_warningLabel->setText( "" );
    m_warningLabel->hide();

    // read the /etc/passwd, fulfill the list.
    readSysUsers(&m_listSysUsers);

    m_illegal = 0;
}

int WizardPage_UserAdd::nextId() const
{
    return Page_Finish;
}

bool WizardPage_UserAdd::validatePage()
{
    if (m_passwd->text().isEmpty()) {
        QMessageBox::warning(this, tr("Root password"), tr("Root password should not be empty."));
        return false;
    }

    bool passwdSame = ( m_passwd->text() == m_confirm->text() );
    if ( !passwdSame ) {
        QMessageBox::warning(this, tr("Root password"), tr("Please input password correctly twice.") );
        return false;
    }

    if (m_user1->text().isEmpty()) {
        QMessageBox::warning(this, tr("User"), tr("Please add normal user.") );
        return false;
    }

    if (m_userPasswd->text() != m_userConfirm->text()) {
        QMessageBox::warning(this, tr("User password"), tr("Please input password correctly twice.") );
        return false;
    }
    return true;
}

void WizardPage_UserAdd::setPasswdUser()
{
    g_engine->cmdSetRootPassword( m_passwd->text().toLatin1() );
    if (!m_user1->text().isEmpty()) 
        g_engine->cmdAddUser( m_user1->text().toUtf8(), m_userPasswd->text().toUtf8() );
}

bool WizardPage_UserAdd::isComplete() const
{
    if ( !m_illegal )
        return true;
    else
        return false;
}

void WizardPage_UserAdd::showWarningLabel()
{
    QString warning = tr("");
    QString name = tr("");
    if ( m_illegal & 0x0001 ) {
        name += m_user1->text();
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

    emit completeChanged();
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
