#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QTextStream>
#include <cassert>
#include "parser_locale.h"

bool ParserLocale::open(const QString& filename)
{
    QString errorMsg;
    int errorLine;
    int errorColumn;

    QFile file(filename);
    if( !file.open( QFile::ReadOnly ) )
	return false;

    if( !m_doc.setContent(&file, true, &errorMsg, &errorLine, &errorColumn) ) {
	qDebug() << "xml file" << filename 
		 << "parser error at line" << errorLine
		 << "column" << errorColumn 
		 << ":" << errorMsg;
	file.close();
	return false;
    }

    m_filename = filename;
    file.close();

    m_root = m_doc.documentElement();
    m_listNode = m_root.elementsByTagName("lang");

    return true;
}

bool ParserLocale::save()
{
    QFile file(m_filename);
    if ( !file.open( QFile::WriteOnly | QFile::Truncate ) ) {
	qDebug() << "ParserLocale Error: can't write file " << m_filename ;
	return false;
    }
    
    QTextStream out(&file);
    m_doc.save( out, 2 ); 	// set indent of xml file to 2.
    file.close();
    return true;
}

QList<QString> ParserLocale::listLocaleEnabled()
{
    QList<QString> list;

    for ( unsigned int i=0 ; i < m_listNode.length() ; i++ )
	{
	    QDomNode node = m_listNode.item(i);
	    if ( node.namedItem("enabled").toElement().text() == "true" )
		list.append( node.namedItem("locale").toElement().text() );
	}
    return list;
}

QList<QString> ParserLocale::listNameEnabled()
{
    QList<QString> list;
    
    for( unsigned int i=0 ; i < m_listNode.length() ; i++ )
	{
	    QDomNode node = m_listNode.item(i);
	    if ( node.namedItem("enabled").toElement().text() == "true" ) {
		list.append( node.namedItem("name").toElement().text() );
	    }
	}
    return list;
}

QString ParserLocale::localeMatched(const QLocale& lc)
{
    QString locale;
    QString name = lc.name();
    QString lang = name.left(name.indexOf('_'));
    
    QList<QString> list = listLocaleEnabled();

    for ( QList<QString>::const_iterator itor = list.begin() ;
	  itor != list.end() ; itor++ )
	{ 
	    if ( *itor == name ) {
		locale = name;
		break;
	    } else if ( *itor == lang ) {
		locale = lang;
	    } 
	}

    if ( locale.isEmpty() ) 
	locale = localeDefault();
    
    return locale;
}
    
QString ParserLocale::localeDefault()
{
    QDomNodeList listDefault = m_root.elementsByTagName("default");
    assert ( listDefault.count() == 1 );

    QDomNode nodeDefault = listDefault.item(0);
    assert( !nodeDefault.isNull() );
    QDomNode nodeParent = nodeDefault.parentNode();
    assert ( !nodeParent.isNull() );
    
    return nodeParent.namedItem("locale").toElement().text();
}
    
QString ParserLocale::nameWithLocale(const QString& locale)
{
    QDomNodeList listLocale = m_root.elementsByTagName("locale");
    
    for (unsigned int i=0; i < listLocale.length() ; i++ )
	{
	    QDomNode nodeLocale = listLocale.item(i);
	    if ( nodeLocale.toElement().text() == locale ) {
		QDomNode nodeParent = nodeLocale.parentNode();
		return nodeParent.namedItem("name").toElement().text();
	    }
	}
	    
    return QString();
}

QString ParserLocale::localeWithName(const QString& name)
{
    QDomNodeList listName = m_root.elementsByTagName("name");
    
    for (unsigned int i=0; i < listName.length() ; i++ )
	{
	    QDomNode nodeName = listName.item(i);
	    if ( nodeName.toElement().text() == name ) {
		QDomNode nodeParent = nodeName.parentNode();
		return nodeParent.namedItem("locale").toElement().text();
	    }
	}
	    
    return QString();
}

bool ParserLocale::isEnabled(const QString& locale)
{
    QDomNodeList listLocale = m_root.elementsByTagName("locale");
    
    for ( unsigned int i=0; i < listLocale.length() ; i++ )
	{
	    QDomNode nodeLocale = listLocale.item(i);
	    if ( nodeLocale.toElement().text() == locale ) {
		QDomNode nodeParent = nodeLocale.parentNode();
		if ( nodeParent.namedItem("enabled").toElement().text() == "true" )
		    return true;
		else
		    return false;
	    }
	}
    return true;
}

void ParserLocale::setName(const QString& locale, const QString& name)
{
    QDomNodeList listLocale = m_root.elementsByTagName("locale");
    
    for ( unsigned int i=0; i < listLocale.length() ; i++ )
	{
	    QDomNode nodeLocale = listLocale.item(i);
	    if ( nodeLocale.toElement().text() == locale ) {
		QDomNode nodeParent = nodeLocale.parentNode();
		QDomNode nodeName = nodeParent.namedItem("name");
		QDomNode nodeText = nodeName.firstChild();
		nodeText.setNodeValue( name );
		break;
	    }
	}
}

void ParserLocale::setEnabled(const QString& locale, bool enabled)
{
    QDomNodeList listLocale = m_root.elementsByTagName("locale");
    
    for ( unsigned int i=0; i < listLocale.length() ; i++ )
	{
	    QDomNode nodeLocale = listLocale.item(i);
	    if ( nodeLocale.toElement().text() == locale ) {
		QDomNode nodeParent = nodeLocale.parentNode();
		QDomNode nodeEnabled = nodeParent.namedItem("enabled");
		QDomNode nodeText = nodeEnabled.firstChild();
		if ( enabled ) 
		    nodeText.setNodeValue( "true" );
		else
		    nodeText.setNodeValue( "false" );
		break;
	    }
	}
}

