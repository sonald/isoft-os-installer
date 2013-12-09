#include "parser_groups.h"
#include <QDebug>
#include <cassert>

GroupsParser::GroupsParser()
{
    m_langs << "en" << "es" << "pt" << "zh";
}

GroupsParser::~GroupsParser()
{
    if ( m_writeFile.isOpen() ) {
	m_writeFile.close();
    }
}

bool GroupsParser::openDestFile(const QString& filename)
{
    m_writeFile.setFileName( filename );
    if ( m_writeFile.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
	m_writer.setDevice(&m_writeFile);
	m_writer.setAutoFormatting(true);
	return true;
    } else
	return false;
}

void GroupsParser::closeDestFile()
{
    if ( m_writeFile.isOpen() ) {
	m_writeFile.close();
    }
}

void GroupsParser::handleDTD()
{
    m_writer.writeCurrentToken( m_reader );
}

void GroupsParser::handleDocStart()
{
    m_writer.writeCurrentToken( m_reader );
}

void GroupsParser::handleDocEnd()
{
}

void GroupsParser::handleCompsStart() 
{
    m_writer.writeCurrentToken( m_reader );
}

void GroupsParser::handleCompsEnd()
{
    m_writer.writeCurrentToken( m_reader );
}
void GroupsParser::handleGroupStart() 
{
    m_writer.writeCurrentToken( m_reader );
}
void GroupsParser::handleGroupEnd()
{
    m_writer.writeCurrentToken( m_reader );
}

void GroupsParser::handleGrpId(const QString& uri, const QString& name, 
	const QString& id)
{
    assert( !id.isEmpty() );
    m_writer.writeTextElement( uri, name, id );
}


void GroupsParser::handleGrpName(const QString& uri, const QString& name, 
	const QString& groupname, const QXmlStreamAttributes& attrs)
{
    bool inLangs = true;
    if ( !attrs.isEmpty() ) {
	QString lang = attrs.value( "xml:lang" ).toString();
	if ( lang.contains('_') )
	    lang.truncate( lang.indexOf('_') );
	inLangs = m_langs.contains( lang );
    }
    assert( !groupname.isEmpty() );
    if ( inLangs ) {
	m_writer.writeStartElement( uri, name );
	m_writer.writeAttributes( attrs );
	m_writer.writeCharacters( groupname );
	m_writer.writeEndElement();
    }
}

void GroupsParser::handleGrpDescription(const QString& uri, const QString& name, 
	const QString& description, const QXmlStreamAttributes& attrs)
{
    bool inLangs = true;
    if ( !attrs.isEmpty() ) {
	QString lang = attrs.value( "xml:lang" ).toString();
	if ( lang.contains('_') )
	    lang.truncate( lang.indexOf('_') );
	inLangs = m_langs.contains( lang );
    }
    if ( inLangs ) {
	if ( description.isEmpty() ) {
	    m_writer.writeEmptyElement( uri, name );
	    m_writer.writeAttributes( attrs );
	} else {
	    m_writer.writeStartElement( uri, name );
	    m_writer.writeAttributes( attrs );
	    m_writer.writeCharacters( description );
	    m_writer.writeEndElement();
	}
    }
}

void GroupsParser::handleDefault(const QString& uri, const QString& name,
	const QString& def)
{
    assert( !def.isEmpty() );
    m_writer.writeTextElement( uri, name, def );
}

void GroupsParser::handleVisible(const QString& uri, const QString& name,
	const QString& visible)
{
    assert( !visible.isEmpty() );
    m_writer.writeTextElement( uri, name, visible );
}
