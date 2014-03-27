#ifndef _archinfo_h
#define _archinfo_h 

#include <sys/utsname.h>
#include <QtCore>

// info about architecture
class ArchInfo: public QObject 
{
    Q_OBJECT
public:
    enum Arch {
        Unknown,
        Loongson,
        AMD64,
        IA32
    };

    QString uname();
    Arch arch(); 
    QString os();
};

#endif
