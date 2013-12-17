#ifndef RFINSTALLER_WIZARDPAGE_SUMMARY_H_
#define RFINSTALLER_WIZARDPAGE_SUMMARY_H_

#include <QWizardPage>
#include "installer_global.h"
#include "wizardpage_partition.h"

class DialogSummary;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpacerItem;
class QString;
class QTextEdit;
class QVBoxLayout;

class WizardPage_Summary : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString pathGrub READ pathGrub WRITE setPathGrub NOTIFY pathGrubChanged() )

public:
    WizardPage_Summary(QWidget* parent =0);
    void initializePage();
    int nextId() const;
    bool validatePage();

    const QString& pathGrub() const;
    void setPathGrub(const QString& path);

signals:
    void pathGrubChanged();

private slots:
    void advancedDialog();

private:
    QLabel	*m_warning;
    QTextEdit	*m_summary; 
    QPushButton *m_advanced;
    QSpacerItem *m_spacerItem;
    QHBoxLayout *m_buttonLayout;
    QVBoxLayout *m_layout;

    QString m_mode;
    QString m_pathGrub;
    WizardPage_Partition* m_page;

    bool m_mbr;
    bool m_instBootLoader;
};
#endif // RFINSTALLER_WIZARDPAGE_SUMMARY_H_
