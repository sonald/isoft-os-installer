#include "wizardpage_packages.h"
#include <QLabel>
#include <QListWidget>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QDebug>

GroupWidgetItem::GroupWidgetItem(QListWidget * parent, int type)
	: QListWidgetItem( parent, type )
{
}

GroupWidgetItem::GroupWidgetItem(const QString & text, QListWidget * parent, int type)
	: QListWidgetItem( text, parent, type )
{
}

GroupWidgetItem::GroupWidgetItem(const QIcon & icon, const QString & text, QListWidget * parent, int type )
    	: QListWidgetItem( icon, text, parent, type )
{
}

const QString& GroupWidgetItem::id()
{
    return m_id;
}

void GroupWidgetItem::setId(const QString& id)
{
    m_id = id;
}

WizardPage_Packages::WizardPage_Packages(QWidget* parent)
    : QWizardPage(parent)
{
    m_labelDescription = new QLabel(this);
    m_labelDescription->setWordWrap(true);
    
    m_listGroups = new QListWidget(this);
    m_listGroups->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listGroups->setLayoutMode(QListView::Batched);
    m_listGroups->setBatchSize(50);
    m_listGroups->setFlow(QListView::TopToBottom);
    m_listGroups->setWrapping(false);
    m_listGroups->setWordWrap(false);

    m_spacer = new QSpacerItem(20,40, QSizePolicy::Minimum, QSizePolicy::Preferred);
    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_listGroups);
    m_layout->addWidget(m_labelDescription);
    m_layout->addItem(m_spacer);
    setLayout(m_layout);

    QFont font;
    font.setPointSize(16);
    m_listGroups->setFont(font);
    m_listGroups->setIconSize(QSize(32,32));

    m_map = new QMap<QString, GroupInfo>;
    connect( m_listGroups, SIGNAL( itemPressed ( QListWidgetItem* ) ),
	    	this, SLOT( updateDesc( QListWidgetItem* ) ) );
    connect( m_listGroups, SIGNAL( itemChanged ( QListWidgetItem* ) ),
	    	this, SLOT( setMapItemChecked( QListWidgetItem* ) ) );
}

void WizardPage_Packages::initializePage()
{
    setTitle( tr("Install Packages") );
    setSubTitle( tr("Select the packages which you want to install.") );
    CustomParser parser(g_appResPath + "/rfgroups.xml");
    parser.createMap(m_map);
    constructListWidget();
}

void WizardPage_Packages::constructListWidget()
{
    QMap<QString, GroupInfo>::const_iterator constItor = m_map->constBegin();
    for ( ; constItor != m_map->constEnd() ; constItor++ )
    {
	QString name = constItor.value().name;
	if ( constItor.value().visible ) {
	    QString id = constItor.key();
	    GroupWidgetItem* item = new GroupWidgetItem( name, m_listGroups );
	    item->setId( id );

	    m_listGroups->addItem( item );
	    if ( constItor.value().def )
		item->setCheckState( Qt::Checked );
	    else
		item->setCheckState( Qt::Unchecked );
	}
    }
}

int WizardPage_Packages::nextId() const
{
    return Page_Summary;
}

void WizardPage_Packages::updateDesc( QListWidgetItem* item )
{
    m_labelDescription->setText(m_map->value( ((GroupWidgetItem*)item)->id() ).description );
}

void WizardPage_Packages::setMapItemChecked( QListWidgetItem* item )
{
    bool checked;
    if ( item->checkState() == Qt::Checked ) 
	checked = true;
    else
	checked = false;

    QString id;
    id = ((GroupWidgetItem*)item)->id();
    QMap<QString, GroupInfo>::iterator itor = m_map->find( id );
    GroupInfo info;
    info.name = itor.value().name;
    info.description = itor.value().description;
    info.iconPath = itor.value().iconPath;
    info.def = itor.value().def;
    info.visible = itor.value().visible;
    info.checked = checked;
    m_map->insert( id, info );
}

void WizardPage_Packages::writeConf()
{
    qDebug() << "Write custom packages.";
    QMap<QString, GroupInfo>::const_iterator constItor = m_map->constBegin();
    for ( ; constItor != m_map->constEnd() ; constItor++ )
    {
	if ( constItor.value().checked )
	    g_engine->cmdAddGroup( constItor.key().toLatin1() );
    }
}
