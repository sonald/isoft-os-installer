#ifndef WIZARDPAGE_CHOOSEGROUP_H
#define WIZARDPAGE_CHOOSEGROUP_H

#include <QWidget>
#include <QWizardPage>
#include <QSignalMapper>

namespace Ui {
class WizardPage_chooseGroup;
}

class WizardPage_chooseGroup : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QStringList selectedGroups READ selectedGroups WRITE setSelectedGroups NOTIFY selectedGroupsChanged)
public:
    explicit WizardPage_chooseGroup(QWidget *parent = 0);
    void initializePage();
    ~WizardPage_chooseGroup();

    const QStringList &selectedGroups() const { return m_selectedGroups; }
    void setSelectedGroups(const QStringList &sl);
    bool validatePage();

signals:
    void selectedGroupsChanged(const QStringList &);

protected slots:
    void handleGroupSelection(QWidget *w);

private:
    Ui::WizardPage_chooseGroup *ui;
    QStringList m_selectedGroups;
    QSignalMapper m_sigMap;

    void loadGroupInfo();
};

#endif // WIZARDPAGE_CHOOSEGROUP_H
