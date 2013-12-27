#ifndef WIZARDPAGE_CHOOSEGROUP_H
#define WIZARDPAGE_CHOOSEGROUP_H

#include <QtGui>

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
    virtual bool isComplete() const;

signals:
    void selectedGroupsChanged(const QStringList &);

protected slots:
    void handleGroupSelection(QWidget *w);

private:
    Ui::WizardPage_chooseGroup *ui;
    QStringList m_selectedGroups;
    QMap<QString, QCheckBox*> m_chkBoxes;
    QSignalMapper m_sigMap;

    void loadGroupInfo();
    QStringList groupsRequired(const QString &group);
};

#endif // WIZARDPAGE_CHOOSEGROUP_H
