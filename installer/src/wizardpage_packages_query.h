#ifndef _WIZARDPAGE_PACKAGES_QUERY_H_
#define _WIZARDPAGE_PACKAGES_QUERY_H_

#include <QWizardPage>
#include "installer_global.h"

class QLabel;
class QRadioButton;
class QString;
class QVBoxLayout;

class WizardPage_Packages_Query : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString packageMode READ packageMode WRITE setPackageMode NOTIFY modeChanged() ); 
public:
    WizardPage_Packages_Query(QWidget *parent =0);
    void initializePage();
    int nextId() const;

    const QString& packageMode() const;
    void setPackageMode(const QString& mode);
signals:
    void modeChanged();
public slots:
    void buttonChecked();

private:
    QString 		m_packageMode;

    QRadioButton* 	m_minButton;
    QLabel*		m_minLabel;
    QRadioButton* 	m_completeButton;
    QLabel*		m_completeLabel;
    QRadioButton*	m_customizeButton;
    QLabel*		m_customizeLabel;
    QVBoxLayout*	m_layout;
};
#endif //_WIZARDPAGE_PACKAGES_QUERY_H_
