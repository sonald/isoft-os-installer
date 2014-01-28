#ifndef _alpm_installer_h
#define _alpm_installer_h 

#include <alpm.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <list>

#include "ipacman.h"

using namespace std;

typedef void (*upstream_report_t)(int);

class AlpmInstaller
{
public:
    //install groups in order
    AlpmInstaller(const list<string> &groups, const string &rootdir);
    ~AlpmInstaller();
    bool install(void (*progress)(int percent) = NULL);
    bool preprocess();
    bool reportUpstream(int);

private:
    list<string> _groups;
    string _rootdir;
    alpm_list_t *_targets;

    int _nr_total_pkgs; // No of pkgs to install of all groups
    upstream_report_t _reporter;
    void collectPkgSize();
};

#endif

