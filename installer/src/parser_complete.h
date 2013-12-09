#ifndef _PARSER_COMPLETE_H_
#define _PARSER_COMPLETE_H_

#include "installer_global.h"
#include "parser_comps.h"
#include <QStringList>

class CompleteParser : public CompsParser
{
public:
    CompleteParser();
    ~CompleteParser();

    void writeConf();
    void printList();

protected:
    virtual void handleGrpId(const QString& uri, const QString& name, 
				const QString& id);
private:
    QStringList m_listGroups;
};
#endif //_PARSER_COMPLETE_H_
