#include <QtCore>
#include <QtGui>
#include "wizardpage_choosegroup.h"
#include "ui_wizardpage_choosegroup.h"
#include "installer_global.h"

WizardPage_chooseGroup::WizardPage_chooseGroup(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardPage_chooseGroup)
{
    ui->setupUi(this);
    //unit MB
    m_groupCapacities.insert("core", 1500);
    m_groupCapacities.insert("base", 4000);
    m_groupCapacities.insert("desktop", 2000);

    registerField("selectedGroups", this, "selectedGroups");
    registerField("requiredSize", this, "requiredSize");
}

void WizardPage_chooseGroup::initializePage()
{
    ui->retranslateUi(this);
    if (!m_selectedGroups.count())
        loadGroupInfo();
    emit completeChanged();
}

void WizardPage_chooseGroup::sanitizeChoices()
{
    QStringList result;

    QStringList prios;
    prios << "desktop" << "base" << "core";
    for (int i = 0; i < prios.length(); ++i) {
        if (m_chkBoxes[prios[i]] && m_chkBoxes.value(prios[i])->isChecked()) {
            QStringList sl = groupsRequired(prios[i]);
            foreach(const QString &g, sl) {
                if (!result.contains(g)) {
                    if (m_chkBoxes.value(g)->checkState() == Qt::Unchecked)
                        m_chkBoxes.value(g)->setCheckState(Qt::PartiallyChecked);
                    result.append(g);
                }
            }
        }
    }
    qDebug() << "final groups chosen: " << result;
    m_selectedGroups = result;
    recalculateCapacity();
}

bool WizardPage_chooseGroup::validatePage()
{
    sanitizeChoices();
    g_engine->cmdChooseGroups(m_selectedGroups.join(",").toUtf8().constData());
    return true;
}

void WizardPage_chooseGroup::cleanupPage()
{
    m_selectedGroups.clear();
    foreach(QCheckBox *box, m_chkBoxes) {
        box->deleteLater();
    }
    m_chkBoxes.clear();
}

WizardPage_chooseGroup::~WizardPage_chooseGroup()
{
    delete ui;
}

void WizardPage_chooseGroup::setSelectedGroups(const QStringList &sl)
{
    m_selectedGroups = sl;
    emit selectedGroupsChanged(sl);
}

void WizardPage_chooseGroup::loadGroupInfo()
{
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(ui->groupBox->layout());
    QStringList sl, exists;
    sl << "core" << "base" << "desktop";

    QDir dir("/var/lib/cetcosinstaller/RPMS");
    QStringList dirs = dir.entryList(QStringList() << "RPMS.*", QDir::Dirs);
    for (int i = 0; i < dirs.length(); ++i) {
        QString dirname = dirs.at(i);
        exists << dirname.replace("RPMS.", "");
    }

    foreach(const QString &grp, sl) {
        if (exists.indexOf(grp) == -1) continue;

        QCheckBox *chkBox = new QCheckBox(grp);
        chkBox->setTristate(true);
        m_chkBoxes.insert(grp, chkBox);
        chkBox->setProperty("groupName", grp);
        connect(chkBox, SIGNAL(clicked()), &m_sigMap, SLOT(map()));
        m_sigMap.setMapping(chkBox, chkBox);
        lay->addWidget(chkBox);

        if (grp == "core") {
            chkBox->setChecked(true);
            m_selectedGroups.append(grp);
        }
    }

    recalculateCapacity();
    connect(&m_sigMap, SIGNAL(mapped(QWidget*)), this, SLOT(handleGroupSelection(QWidget*)));
}

QStringList WizardPage_chooseGroup::groupsRequired(const QString &group)
{
    QStringList sl;
    if (group == "core")
        return sl << "core";
    else if (group == "base")
        return sl << "core" << "base";
    else if (group == "desktop")
        return sl << "core" << "base" << "desktop";
    return sl;
}

void WizardPage_chooseGroup::handleGroupSelection(QWidget *w)
{
    QCheckBox *chkBox = qobject_cast<QCheckBox*>(w);
    QString val = chkBox->property("groupName").toString();
    if (chkBox->isChecked()) {
        QStringList required = groupsRequired(val);
        foreach (const QString &g, required) {
            m_chkBoxes[g]->setChecked(true);
        }

        m_selectedGroups = required;
    } else {
        m_selectedGroups.removeAll(val);
    }

    sanitizeChoices();
    emit completeChanged();
    qDebug() << __PRETTY_FUNCTION__ << m_selectedGroups;
}

void WizardPage_chooseGroup::recalculateCapacity()
{
    int capacity = 0;
    foreach(const QString &grp, m_selectedGroups) {
        capacity += m_groupCapacities.value(grp);
    }

    m_requiredSize = capacity;
    ui->capacityLabel->setText(tr("minimal disk space required now is %1MB").arg(capacity));
}

bool WizardPage_chooseGroup::isComplete() const
{
    return m_selectedGroups.contains("core");
}
