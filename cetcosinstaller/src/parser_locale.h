#ifndef RFINSTALLER_PARSER_LOCALE_H_
#define RFINSTALLER_PARSER_LOCALE_H_

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QList>
#include <QString>

class QLocale;

/* 
 * Rflocale Parser: parser the rflocale.xml into dom and give some relative 
 * service about rflocale.
 * rflocale.xml describe the all locale used for cetcosinstaller I18n. 
 * Rflocale is not real locale, is just some string same as language or
 * language_territory.
 */
class ParserLocale {
 public:
    ParserLocale() { m_filename = QString(); }
    ~ParserLocale(){}

    // Open the xml file.
    bool open(const QString& filename);
    // Save the DOM into same file opened.
    bool save();  

    // List all enabled locales.
    // Locale supported by translate files is enabled.
    QList<QString> listLocaleEnabled();
    // if name is exclusive, listNameEnabled() is safe.
    QList<QString> listNameEnabled();
    
    // Find the rflocale matched the QLocale. 
    // Especially used for find the system locale. 
    QString localeMatched(const QLocale& lc);
    
    // Get the default locale if not matched.
    QString localeDefault();
    
    // Get the name of locale.
    QString nameWithLocale(const QString& locale);
    // Get the locale which have name.
    QString localeWithName(const QString& name);
    // whether locale is enabled.
    bool isEnabled(const QString& locale);

    // Set the name of locale.
    void setName(const QString& locale, const QString& name);
    // Set the enabled state of locale.
    void setEnabled(const QString& locale, bool enabled);

    // Internal debug fun.
    void print();
    
 private:
    QString m_filename;

    QDomDocument m_doc;
    QDomElement m_root;
    QDomNodeList m_listNode;
};
#endif //RFINSTALLER_PARSER_LOCALE_H_
