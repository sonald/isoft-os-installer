#ifndef _PARSER_GROUPS_H_
#define _PARSER_GROUPS_H_

#include "parser_comps.h"
#include <QStringList>
#include <QXmlStreamWriter>

class GroupsParser : public CompsParser
{
public:
    GroupsParser();
    ~GroupsParser();
    bool openDestFile(const QString& filename);
    void closeDestFile();

protected:
    virtual void handleDocStart();
    virtual void handleDocEnd();
    virtual void handleDTD();

    virtual void handleCompsStart();
    virtual void handleCompsEnd();
    virtual void handleGroupStart();
    virtual void handleGroupEnd();

    virtual void handleGrpId(const QString& uri, const QString& name, 
				const QString& id);
    virtual void handleGrpName(const QString& uri, const QString& name, 
				const QString& groupname, const QXmlStreamAttributes& attrs);
    virtual void handleGrpDescription(const QString& uri, const QString& name, 
				const QString& description, const QXmlStreamAttributes& attrs);
    virtual void handleDefault(const QString& uri, const QString& name,
	    			const QString& def);
    virtual void handleVisible(const QString& uri, const QString& name,
	    			const QString& visible);
private:
    QStringList m_langs;
    QFile	m_writerFile;
    QXmlStreamWriter m_writer;
};
#endif //_PARSER_GROUPS_H_
