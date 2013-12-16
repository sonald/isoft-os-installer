#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>

#include "installengine.h"
using namespace std;

void progress(int percent)
{
    cout << percent << endl;
}

int main(int argc, char* argv[])
{
    Engine *peng = NULL;
    
    if(argc == 3 && string(argv[1]) == "-w") {
        peng = Engine::instance(Engine::WriteConf, argv[2]);
	/*
        peng->cmdMakeLabel("/dev/sda", "ext3");
        peng->cmdMakePartWhole("/dev/sda2", "3", "logical", "ext3");
        peng->cmdMakePartLength("/dev/sda2", "3", "logical", "ext3", "1024");
        peng->cmdRemovePart("/dev/sda2");
        peng->cmdRemoveAllPart("/dev/sda");
        peng->cmdMakeFileSystem("/dev/sda2", "ext3");
        peng->cmdSetMountPoint("/dev/sda2", "/", "ext3");
        peng->cmdAddGroup("base");
        peng->cmdAddPackage("glib");
        peng->cmdSetBootEntry("/dev/sda");
        peng->cmdSetLang("en");
        peng->cmdAddUser("wo");
        peng->cmdSetRootPassword("abc123");
	*/
    }else if(argc == 2){
        peng = Engine::instance(Engine::ReadConf, argv[1]);
    }else{
        cout << "\n\tUsage: cetcosinstaller_console <conf_file>\n\n" << endl;
    }

    if(peng->install(progress)){
        if(peng->postscript()){
            cout << "Install is Succesful." << endl;
        }else{
            cout << "Postscript error: ";
            cout << peng->getErr() << endl;    
        }
    }else{
        cout << "Install error: ";
        cout << peng->getErr() << endl;
    }

    delete peng;
    
    return 0;
}
