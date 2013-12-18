#ifndef RFINSTALLER_WIZARDPAGE_USERADD_H_
#define RFINSTALLER_WIZARDPAGE_USERADD_H_

#include <QStringList>
#include <QValidator>
#include <QWizardPage>
#include "installer_global.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSpacerItem;
class QVBoxLayout;

class WizardPage_UserAdd : public QWizardPage
{
    Q_OBJECT
public:
    WizardPage_UserAdd(QWidget * parent =0);
    void initializePage();
    int  nextId() const;
    bool validatePage();
    bool isComplete() const;

    void setPasswdUser();

private slots:
    void checkUser1(const QString& text);
    void checkUser2(const QString& text);
    void checkUser3(const QString& text);
    void showWarningLabel();

private:
    bool checkUserNameDual();
    void readSysUsers(QStringList* list);

    QStringList         m_listSysUsers;
    int                 m_illegal;
    bool                m_dual;

    QLabel* 		m_rootDescript;
    QLabel* 		m_rootPicture;
    QLabel* 		m_passwdLabel;
    QLabel*		    m_confirmLabel; 
    QLineEdit* 		m_passwd;
    QLineEdit*		m_confirm;
    QHBoxLayout*	m_descriptLayout;
    QGridLayout*	m_passwdLayout;
    QVBoxLayout*	m_rootLayout;

    QLabel*		    m_userPicture;
    QLabel*		    m_userLabel;
    QLineEdit*		m_user1;
    QLineEdit*		m_user2;
    QLineEdit*		m_user3;
    QLabel*         m_star;
    QLabel*         m_warningLabel;
    QHBoxLayout*	m_labelLayout;
    QGridLayout*	m_userLayout;

    QSpacerItem*	m_spacer;
    QVBoxLayout*	m_formLayout;
};

class NameValidator : public QValidator
{
    Q_OBJECT
public:
    NameValidator(QObject* parent) : QValidator(parent) {}
    State validate(QString& input, int& pos) const;
    bool isValid(const QChar& ch) const;
};
#endif // RFINSTALLER_WIZARDPAGE_USERADD_H_
