#include "parser_custom.h"
#include <QDebug>
#include <cassert>
#include <QString>

CustomParser::CustomParser(const QString& filename)
{
    openFile( filename );
}

CustomParser::~CustomParser()
{
    closeFile();
}

void CustomParser::printMap()
{
    QMap<QString, GroupInfo>::const_iterator constItor = m_mapGroupInfo->constBegin();
    for( ; constItor != m_mapGroupInfo->constEnd() ; constItor++ )
    {
	qDebug() << "------------------------------------------------------------------";
	qDebug() << constItor.key();
	qDebug() << "-----------------------------------";
	qDebug() << constItor.value().name;
	qDebug() << constItor.value().description;
	qDebug() << "default: " << ( constItor.value().def ? "true" : "false" ) ;
	qDebug() << "visible: " << ( constItor.value().visible ? "true" : "false" ) ;
    }
}

void CustomParser::createMap(QMap<QString, GroupInfo>* map)
{
    m_mapGroupInfo = map;
    run();
}

void CustomParser::handleGroupStart()
{
    m_curId.clear();
    m_curInfo.name.clear();
    m_curInfo.description.clear();
    m_curInfo.iconPath.clear();
    m_curInfo.def = false;
    m_curInfo.visible = false;
    m_curInfo.checked = false;
}

void CustomParser::handleGroupEnd()
{
    m_mapGroupInfo->insert( m_curId, m_curInfo );
}

void CustomParser::handleGrpId(const QString& , const QString& , 
	const QString& id)
{
    assert( !id.isEmpty() );
    m_curId = id;
}


void CustomParser::handleGrpName(const QString& , const QString& , 
	const QString& groupname, const QXmlStreamAttributes& attrs)
{
    if ( attrs.isEmpty() ) 
	m_curInfo.name = groupname;
}

void CustomParser::handleGrpDescription(const QString& , const QString& , 
	const QString& description, const QXmlStreamAttributes& attrs)
{
    if ( attrs.isEmpty() )
	m_curInfo.description = description;
}

void CustomParser::handleDefault(const QString& , const QString& ,
	const QString& def)
{
    assert( !def.isEmpty() );
    if ( def == "true" )
	m_curInfo.def = true;
    else
	m_curInfo.def = false;
    m_curInfo.checked = m_curInfo.def;
}

void CustomParser::handleVisible(const QString& , const QString& ,
	const QString& visible)
{
    assert( !visible.isEmpty() );
    if ( visible == "true" )
	m_curInfo.visible = true;
    else
	m_curInfo.visible = false;
}

void CustomParser::dispatchGroup()
{
    QString name = m_reader.name().toString();
    qDebug() << name;
    if ( name == "id" )
	parseGrpId();
    else if ( name == "name" )
	parseGrpName();
    else if ( name == "description" )
	parseGrpDescription();
    else if ( name == "default" )
	parseDefault();
    else if ( name == "uservisible" )
	parseVisible();
    else if ( name == "icon" )
	parseIcon();
    else
	parseUnknown();
}

void CustomParser::parseIcon()
{
    assert( m_reader.isStartElement() );

    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString icon;
    if ( !m_reader.atEnd() )
	icon = m_reader.readElementText();

    assert( !icon.isEmpty() );
    handleGrpId( uri, name, icon );
}

void CustomParser::handleIcon(const QString& , const QString& , const QString& icon)
{
    assert( !icon.isEmpty() );
    m_curInfo.iconPath = icon;
}
