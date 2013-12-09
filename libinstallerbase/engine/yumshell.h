#ifndef YUMSHELL_H
#define YUMSHELL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;

class YumShell 
{
public:
    // if the strings in pkgs start with '@', it means it is a group name,
    // if not, it is a pkg name.
    YumShell(const list<string> &pkgs, const string &rootdir, const string &conffile = "");
    ~YumShell();
    int getInstallNumber(void);
    void setProgressRange(int min, int max);
    bool install(void (*progress)(int percent) =NULL);
    const char *getErr(void){ return _errstr.c_str(); }

private:
    bool getRealPkgs(const list<string> &pkgs);
    static bool test(void);

private:
    list<string> _pkgs;
    string _rootdir;
    string _conffile;
    string _errstr;
    int _progmin;
    int _progmax;
};

#endif
