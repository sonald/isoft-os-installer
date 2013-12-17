#ifndef RFINSTALLER_WIZARDPAGE_WELCOME_H_
#define RFINSTALLER_WIZARDPAGE_WELCOME_H_

#include <QHash>
#include <QWizardPage>
#include "installer_global.h"
#include "parser_locale.h"

class QHBoxLayout;
class QLabel;
class QListWidget;
class QString;

/*
 * Welcome Page
 * Set the language and display the welcome sentence.
 */
class WizardPage_Welcome : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString locale READ locale WRITE setLocale NOTIFY localeChanged() )

 public:
    WizardPage_Welcome(QWidget* parent =0);
    void initializePage();
    int nextId() const;
    
    void setLocale(const QString& locale);
    const QString& locale() const;

 signals:
    void localeChanged();

 private slots:
    void updateLocale(const QString & currentText);
    void reTranslateUi();

 private:
    QListWidget* m_listWidget;
    QLabel * m_label;
    QHBoxLayout * m_layout;
    
    ParserLocale m_localeParser;
    QString m_locale;
};
#endif // RFINSTALLER_WIZARDPAGE_WELCOME_H_
