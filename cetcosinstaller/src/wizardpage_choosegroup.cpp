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
    registerField("selectedGroups", this, "selectedGroups");
}

void WizardPage_chooseGroup::initializePage()
{
    loadGroupInfo();
}

bool WizardPage_chooseGroup::validatePage()
{
    if (m_selectedGroups.contains("core")) {
        qDebug() << (m_selectedGroups.join(",").toUtf8().constData());
        g_engine->cmdChooseGroups(m_selectedGroups.join(",").toUtf8().constData());
        return true;
    }

    return false;
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
    sl << "core" << "base" << "extra";

    QDir dir("/var/lib/cetcosinstaller/RPMS");
    QStringList dirs = dir.entryList(QStringList() << "RPMS.*", QDir::Dirs);
    for (int i = 0; i < dirs.length(); ++i) {
        QString dirname = dirs.at(i);
        exists << dirname.replace("RPMS.", "");
    }

    foreach(const QString &grp, sl) {
        if (exists.indexOf(grp) == -1) continue;

        QCheckBox *chkBox = new QCheckBox(grp);
        chkBox->setProperty("groupName", grp);
        connect(chkBox, SIGNAL(clicked()), &m_sigMap, SLOT(map()));
        m_sigMap.setMapping(chkBox, chkBox);
        lay->addWidget(chkBox);

        if (grp == "core") {
            chkBox->setChecked(true);
            m_selectedGroups.append(grp);
        }
    }

    connect(&m_sigMap, SIGNAL(mapped(QWidget*)), this, SLOT(handleGroupSelection(QWidget*)));
}

void WizardPage_chooseGroup::handleGroupSelection(QWidget *w)
{
    qDebug() << __PRETTY_FUNCTION__;
    QCheckBox *chkBox = qobject_cast<QCheckBox*>(w);
    QString val = chkBox->property("groupName").toString();
    if (chkBox->isChecked()) {
        m_selectedGroups << val;
    } else {
        m_selectedGroups.removeAll(val);
    }

}
