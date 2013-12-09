#include "parser_complete.h"
#include <installengine.h>
#include <QDebug>
#include <cassert>

CompleteParser::CompleteParser()
{
}

CompleteParser::~CompleteParser()
{
}

void CompleteParser::handleGrpId(const QString& , const QString& , 
	const QString& id)
{
    assert( !id.isEmpty() );
    m_listGroups << id;
}

void CompleteParser::printList()
{
    QStringList::const_iterator const_itor = m_listGroups.constBegin();
    for( ; const_itor != m_listGroups.constEnd() ; const_itor++ )
    {
	qDebug() << *const_itor;
    }
}

void CompleteParser::writeConf()
{
    QStringList::const_iterator const_itor = m_listGroups.constBegin();
    for( ; const_itor != m_listGroups.constEnd() ; const_itor++ )
    {
	qDebug() << (*const_itor);
	g_engine->cmdAddGroup( (*const_itor).toLatin1() );
    }
}
