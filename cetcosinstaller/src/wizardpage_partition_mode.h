#ifndef RFINSTALLER_WIZARDPAGE_PARTITION_MODE_H_
#define RFINSTALLER_WIZARDPAGE_PARTITION_MODE_H_

#include <QWizardPage>
#include "installer_global.h"

class QLabel;
class QRadioButton;
class QString;
class QVBoxLayout;

class WizardPage_Partition_Mode : public QWizardPage 
{
    Q_OBJECT
    Q_PROPERTY(QString partitionMode READ partitionMode WRITE setPartitionMode NOTIFY modeChanged() )
 public:
    WizardPage_Partition_Mode(QWidget *parent =0);
    void initializePage();
    int nextId() const;
    bool validatePage();

    void setPartitionMode(const QString& mode);
    const QString& partitionMode() const;

 signals:
    void modeChanged();

 private slots:
    void checkHDInstall();
 private:
    QString		m_partitionMode;

    QRadioButton 	*m_autoButton;
    QLabel		*m_autoLabel;
    QRadioButton 	*m_simpleButton;
    QLabel		*m_simpleLabel;
    QRadioButton 	*m_advancedButton;
    QLabel		*m_advancedLabel;
    QVBoxLayout		*m_layout;
};
#endif // RFINSTALLER_WIZARDPAGE_PARTITION_MODE_H_
