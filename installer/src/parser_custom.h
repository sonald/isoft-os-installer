#ifndef _PARSER_CUSTOM_H_
#define _PARSER_CUSTOM_H_

#include "parser_comps.h"
#include <QMap>

class QString;

struct GroupInfo {
    QString name;
    QString description;
    QString iconPath;
    bool def;
    bool visible;
    bool checked;
};

class CustomParser : public CompsParser
{
public:
    CustomParser(const QString& filename);
    ~CustomParser();

    void printMap();	// For debug
    void createMap(QMap<QString, GroupInfo> *map);

protected:
    virtual void handleGroupStart();
    virtual void handleGroupEnd();
    virtual void dispatchGroup();
    void parseIcon();
    void handleIcon(const QString&, const QString&, const QString& );

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
    QMap<QString, GroupInfo>* m_mapGroupInfo;
    QString 		m_curId;
    GroupInfo 		m_curInfo;
};
#endif //_PARSER_CUSTOM_H_
