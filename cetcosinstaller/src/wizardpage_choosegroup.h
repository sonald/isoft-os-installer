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
    Q_PROPERTY(int requiredSize READ requiredSize WRITE setRequiredSize NOTIFY requiredSizeChanged)

public:
    explicit WizardPage_chooseGroup(QWidget *parent = 0);
    void initializePage();
    ~WizardPage_chooseGroup();

    const QStringList &selectedGroups() const { return m_selectedGroups; }
    void setSelectedGroups(const QStringList &sl);

    int requiredSize() const { return m_requiredSize; }
    void setRequiredSize(int val) { m_requiredSize = val; emit requiredSizeChanged(val); }

    bool validatePage();
    virtual bool isComplete() const;

signals:
    void selectedGroupsChanged(const QStringList &);
    void requiredSizeChanged(int);

protected slots:
    void handleGroupSelection(QWidget *w);

private:
    Ui::WizardPage_chooseGroup *ui;
    QStringList m_selectedGroups;
    QMap<QString, QCheckBox*> m_chkBoxes;
    QMap<QString, int> m_groupCapacities;
    QSignalMapper m_sigMap;
    int m_requiredSize;

    void loadGroupInfo();
    QStringList groupsRequired(const QString &group);
    void recalculateCapacity();
    void sanitizeChoices();
};

#endif // WIZARDPAGE_CHOOSEGROUP_H
