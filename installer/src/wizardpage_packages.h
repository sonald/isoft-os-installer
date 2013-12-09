#ifndef _WIZARDPAGE_PACKAGES_H_
#define _WIZARDPAGE_PACKAGES_H_

#include "installer_global.h"
#include "parser_custom.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QWizardPage>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QSpacerItem;
class QIcon;

class GroupWidgetItem : public QListWidgetItem 
{
public:
    GroupWidgetItem ( QListWidget * parent = 0, int type = Type );
    GroupWidgetItem ( const QString & text, QListWidget * parent = 0, int type = Type );
    GroupWidgetItem ( const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type );
    const QString& id();
    void setId(const QString& id);

private:
    QString m_id;
};

class WizardPage_Packages : public QWizardPage 
{
Q_OBJECT
public:
    WizardPage_Packages(QWidget *parent=0);
    void initializePage();
    int nextId() const;
    void writeConf();
private slots:
    void updateDesc( QListWidgetItem* );
    void setMapItemChecked ( QListWidgetItem* );
private:
    void constructListWidget();

    QListWidget* 	m_listGroups;
    QLabel*		m_labelDescription;
    QSpacerItem*	m_spacer;
    QVBoxLayout*	m_layout;
    QMap<QString, GroupInfo>*	m_map;
};
#endif //_WIZARDPAGE_PACKAGES_H_
