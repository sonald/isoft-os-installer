#include "parser_comps.h"
#include <QDebug>
#include <cassert>

CompsParser::CompsParser()
{
}

CompsParser::~CompsParser()
{
    if ( m_readerFile.isOpen() ) {
	m_reader.clear();
	m_readerFile.close();
    }
}

bool CompsParser::openFile(const QString& filename)
{
    m_readerFile.setFileName( filename );
    if ( m_readerFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
	m_reader.setDevice(&m_readerFile);
	return true;
    } else
	return false;
}

void CompsParser::closeFile()
{
    if ( m_readerFile.isOpen() ) {
	m_reader.clear();
	m_readerFile.close();
    }
}

// handle error.
bool CompsParser::hasError()
{
    return m_reader.hasError();
}

QString CompsParser::errorType()
{
    QXmlStreamReader::Error error_type = m_reader.error();
    switch ( error_type ) {
	case QXmlStreamReader::CustomError :
		return  "CustomError";
	case QXmlStreamReader::NotWellFormedError :
		return  "NotWellFormedError";
	case QXmlStreamReader::PrematureEndOfDocumentError :
		return  "PrematureEndOfDocumentError ";
	case QXmlStreamReader::UnexpectedElementError :
		return  "UnexpectedElementError";
	case QXmlStreamReader::NoError:
		return  "Should be never happened";
	default:
		return "Never happen is happened.";
    } 
}

QString CompsParser::errorString()
{
    return m_reader.errorString();
}

void CompsParser::echoError(const QString& error)
{
    QString line;
    line.setNum( m_reader.lineNumber() );
    QString lineError = QString(" %1 :").arg(line, 5, '0') + error;
    m_reader.raiseError( lineError );
}

// run, parse the whole file.
void CompsParser::run()
{
    while( !m_reader.atEnd() ) {
	QXmlStreamReader::TokenType type = m_reader.readNext();
	switch ( type ) {
	    case QXmlStreamReader::NoToken : break;
	    case QXmlStreamReader::Invalid : break;
	    case QXmlStreamReader::Comment : break;
	    case QXmlStreamReader::Characters: 
		 {
		     qDebug() << m_reader.text().toString();
		     if ( !m_reader.isWhitespace() )
			 echoError("unexpected characters.");
		     break;
		 }
	    case QXmlStreamReader::DTD :
		 {
		     handleDTD();
		     break;
		 }
	    case QXmlStreamReader::StartElement :
		 {
		     QString name = m_reader.name().toString();
		     qDebug() << name;
		     if ( name == "comps" )
			 parseComps();
		     else 
			 echoError("top tag is not comps.");
		     break;
		 }
					     /*
	    case QXmlStreamReader::EndElement: break; // never happen.
	    case QXmlStreamReader::EntityReference:
					     qDebug() << "EntityReference";
					     break;
	    case QXmlStreamReader::ProcessingInstruction:
					     qDebug() << "ProcessingInstruction";
					     break;
					     */
	    case QXmlStreamReader::StartDocument :
		 {
		     handleDocStart();
		     break;
		 }
	    case QXmlStreamReader::EndDocument :
		 {
		     handleDocEnd();
		     break;
		 }
	    default: echoError("unexpected tag");
		     break;
	}
    }

    if ( ! hasError() )
	qDebug() << "Success!";
    else {
	qDebug() << errorType();
	if ( errorType() == "CustomError" )
	    qDebug() << errorString();
    }
}

void CompsParser::handleDTD()
{
}

void CompsParser::handleDocStart()
{
}

void CompsParser::handleDocEnd()
{
}

void CompsParser::parseComps()
{
    assert( m_reader.isStartElement() );
    handleCompsStart();

    while( ! m_reader.atEnd() ) {
	QXmlStreamReader::TokenType type = m_reader.readNext();

	if ( QXmlStreamReader::EndElement == type ) {
	    QString endname = m_reader.name().toString();
	    if ( endname != "comps" )
		echoError("unexpected comps end");
	    handleCompsEnd();
	    break;
	}

	switch ( type ) {
	    case QXmlStreamReader::Comment : break;
	    case QXmlStreamReader::Characters:
		{
		    qDebug() << m_reader.text().toString();
		    if ( !m_reader.isWhitespace() )
			echoError("unexpected characters");
		    break;
		}
	    case QXmlStreamReader::StartElement :
		{
		    dispatchComps();
		    break;
		}
	    default: echoError("unexpected tag in comps");
		     break;
	}
    }
}

void CompsParser::dispatchComps()
{
    QString name = m_reader.name().toString();
    qDebug() << name;
    if ( name == "group" )
	parseGroup();
    else if ( name == "category" )
	parseCategory();
    else
	echoError("unexpect start element in comps");
}

void CompsParser::parseGroup()
{
    assert( m_reader.isStartElement() );
    handleGroupStart();
    
    while ( ! m_reader.atEnd() ) {
	QXmlStreamReader::TokenType type = m_reader.readNext();

	if ( type == QXmlStreamReader::EndElement ) {
	    QString endname = m_reader.name().toString();
	    if ( endname != "group" )
		m_reader.raiseError("Unexpected group end.");
	    handleGroupEnd();
	    break;
	}

	switch ( type ) {
	    case QXmlStreamReader::Comment : break;
	    case QXmlStreamReader::Characters:
		{
		    qDebug() << m_reader.text().toString();
		    if ( !m_reader.isWhitespace() )
			echoError("unexpected characters");
		    break;
		}
	    case QXmlStreamReader::StartElement :
		{
		    dispatchGroup();
		    break;
		}
	    default: echoError("unexpected tag in group");
		     break;
	}
    }
}

void CompsParser::dispatchGroup()
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
    else if ( name == "packagelist" )
	parsePackageList();
    else
	parseUnknown();
}

void CompsParser::parseGrpId()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString id;
    if ( !m_reader.atEnd() )
	id = m_reader.readElementText();
    
    assert( !id.isEmpty() );
    handleGrpId( uri, name, id );
}

void CompsParser::parseGrpName()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();
    
    QXmlStreamAttributes attrs = m_reader.attributes();
    
    QString groupname;
    if ( !m_reader.atEnd() )
	groupname = m_reader.readElementText();

    assert( !groupname.isEmpty() );
    handleGrpName( uri, name, groupname, attrs );
}

void CompsParser::parseGrpDescription()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();
    
    QXmlStreamAttributes attrs = m_reader.attributes();
    
    QString description;
    if ( !m_reader.atEnd() ) 
	description = m_reader.readElementText();

    handleGrpDescription( uri, name, description, attrs );
}

void CompsParser::parseDefault()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString def;
    if ( !m_reader.atEnd() )
	def = m_reader.readElementText();
    
    assert( !def.isEmpty() );
    handleDefault( uri, name, def );
}

void CompsParser::parseVisible()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString visible;
    if ( !m_reader.atEnd() )
	visible = m_reader.readElementText();
    
    assert( !visible.isEmpty() );
    handleVisible( uri, name, visible );
}


void CompsParser::parseUnknown()
{
    assert( m_reader.isStartElement() );
    
    while( !m_reader.atEnd() ) {
	m_reader.readNext();
	if ( m_reader.isEndElement() ) 
	    break;
	
	if ( m_reader.isStartElement() )
	    parseUnknown();
    }
}

void CompsParser::parsePackageList()
{
    parseUnknown();
}

void CompsParser::parseCategory()
{
    assert( m_reader.isStartElement() );
    handleCategoryStart();
    
    while ( ! m_reader.atEnd() ) {
	QXmlStreamReader::TokenType type = m_reader.readNext();

	if ( type == QXmlStreamReader::EndElement ) {
	    QString endname = m_reader.name().toString();
	    if ( endname != "category" )
		m_reader.raiseError("Unexpected category end.");
	    handleCategoryEnd();
	    break;
	}

	switch ( type ) {
	    case QXmlStreamReader::Comment : break;
	    case QXmlStreamReader::Characters:
		{
		    qDebug() << m_reader.text().toString();
		    if ( !m_reader.isWhitespace() )
			echoError("unexpected characters");
		    break;
		}
	    case QXmlStreamReader::StartElement :
		{
		    dispatchCategory();
		    break;
		}
	    default: echoError("unexpected tag in group");
		     break;
	}
    }
}

void CompsParser::dispatchCategory()
{
    QString name = m_reader.name().toString();
    qDebug() << name;
    if ( name == "id" )
	parseCatId();
    else if ( name == "name" )
	parseCatName();
    else if ( name == "description" )
	parseCatDescription();
    else if ( name == "display_order" )
	parseDisplayOrder();
    else if ( name == "grouplist" )
	parseGroupList();
    else
	parseUnknown();
}

void CompsParser::parseCatId()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString id;
    if ( !m_reader.atEnd() )
	id = m_reader.readElementText();
    
    assert( !id.isEmpty() );
    handleCatId( uri, name, id );
}

void CompsParser::parseCatName()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();
    
    QXmlStreamAttributes attrs = m_reader.attributes();
    
    QString groupname;
    if ( !m_reader.atEnd() )
	groupname = m_reader.readElementText();

    assert( !groupname.isEmpty() );
    handleCatName( uri, name, groupname, attrs );
}

void CompsParser::parseCatDescription()
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();
    
    QXmlStreamAttributes attrs = m_reader.attributes();
    
    QString description;
    if ( !m_reader.atEnd() ) 
	description = m_reader.readElementText();

    handleCatDescription( uri, name, description, attrs );
}

void CompsParser::parseDisplayOrder() 
{
    assert( m_reader.isStartElement() );
    
    QString uri = m_reader.namespaceUri().toString();
    QString name = m_reader.name().toString();

    QString order;
    if ( !m_reader.atEnd() )
	order = m_reader.readElementText();
    
    assert( !order.isEmpty() );
    handleDisplayOrder( uri, name, order );
}

void CompsParser::parseGroupList() 
{
    parseUnknown();
}
// Normally not implemented.
void CompsParser::handleCompsStart() {}
void CompsParser::handleCompsEnd() {}
void CompsParser::handleGroupStart() {}
void CompsParser::handleGroupEnd() {}
void CompsParser::handleCategoryStart() {}
void CompsParser::handleCategoryEnd() {}

// Should be implemented in derived class.
void CompsParser::handleGrpId(const QString& , const QString& , 
	const QString& )
{
}
void CompsParser::handleGrpName(const QString& , const QString& , 
	const QString& , const QXmlStreamAttributes& )
{
}
void CompsParser::handleGrpDescription(const QString& , const QString& , 
	const QString& , const QXmlStreamAttributes& )
{
}
void CompsParser::handleDefault(const QString& , const QString& ,
	const QString& )
{
}
void CompsParser::handleVisible(const QString& , const QString& ,
	const QString& )
{
}

void CompsParser::handleCatId(const QString& , const QString& ,	const QString& ) 
{
}
void CompsParser::handleCatName(const QString&, const QString&,
	const QString&, const QXmlStreamAttributes&)
{
}
void CompsParser::handleCatDescription(const QString& , const QString& ,
	const QString& , const QXmlStreamAttributes& )
{
}

void CompsParser::handleDisplayOrder( const QString& , const QString& , const QString& )
{
}
