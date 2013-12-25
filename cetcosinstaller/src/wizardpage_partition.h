#ifndef RFINSTALLER_WIZARDPAGE_PARTITION_H_
#define RFINSTALLER_WIZARDPAGE_PARTITION_H_

#include <QWizardPage>
#include "installer_global.h"

class QString;

class WizardPage_Partition : public QWizardPage 
{
Q_OBJECT
public:
    WizardPage_Partition(QWidget* parent=0) : QWizardPage(parent) {}
    virtual const QString 	finalPartInfo() =0;
    virtual const QString 	warningInfo()	=0;
    virtual const QString 	rootPartPath()	=0;
    virtual void 		writeConf()	=0;
    int nextId() const { return Page_ChooseGroup; }
};
#endif // RFINSTALLER_WIZARDPAGE_PARTITION_H_
