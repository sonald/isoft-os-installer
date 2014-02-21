#include "archinfo.h"

QString ArchInfo::uname() 
{
    struct utsname uts;
    ::uname(&uts);
    QString s(uts.machine);
    return s.toLower();
}

ArchInfo::Arch ArchInfo::arch() 
{
    QString s = this->uname();
    if (s.contains("mips")) return Loongson;
    else if (s.contains("x86_64")) return AMD64;
    else return IA32;
}

QString ArchInfo::os() 
{
    switch(this->arch()) {
        case Loongson: return tr("iSoft Client OS for Loongson V1.0");
        default: return tr("iSoft Client OS V3.0");
    }
}
