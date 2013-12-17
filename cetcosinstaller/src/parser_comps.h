#ifndef _COMPS_PARSER_H_
#define _COMPS_PARSER_H_

#include <QFile>
#include <QString>
#include <QXmlStreamReader>

class CompsParser
{
public:
    CompsParser();
    ~CompsParser();
    bool openFile(const QString& filename);
    void closeFile();
    void run();
    bool hasError();
    QString errorType();
    QString errorString();

protected:
    virtual void handleDocStart();
    virtual void handleDocEnd();
    virtual void handleDTD();
    
    virtual void parseComps();
    virtual void handleCompsStart();
    virtual void handleCompsEnd();
    virtual void dispatchComps();
    
    virtual void parseGroup();
    virtual void handleGroupStart();
    virtual void handleGroupEnd();
    virtual void dispatchGroup();
    
    virtual void parseCategory();
    virtual void handleCategoryStart();
    virtual void handleCategoryEnd();
    virtual void dispatchCategory();
    
    virtual void parseUnknown();
    
    virtual void parseGrpId();
    virtual void parseGrpName();
    virtual void parseGrpDescription();
    virtual void parseDefault();
    virtual void parseVisible();
    virtual void parsePackageList();


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

    virtual void parseCatId();
    virtual void parseCatName();
    virtual void parseCatDescription();
    virtual void parseDisplayOrder();
    virtual void parseGroupList();

    virtual void handleCatId(const QString& uri, const QString& name, 
				const QString& id);
    virtual void handleCatName(const QString& uri, const QString& name, 
				const QString& groupname, const QXmlStreamAttributes& attrs);
    virtual void handleCatDescription(const QString& uri, const QString& name, 
				const QString& description, const QXmlStreamAttributes& attrs);
    virtual void handleDisplayOrder(const QString& uri, const QString& name, const QString& order);

    void echoError(const QString& error);
    
    QXmlStreamReader m_reader;
    
    QString 	m_errorString;
    QFile	m_readerFile;	
};
#endif //_COMPS_PARSER_H_
