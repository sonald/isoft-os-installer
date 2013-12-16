#ifndef _RPM_INSTALLER_H
#define _RPM_INSTALLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#include <rpm/rpmcli.h>

using namespace std;

typedef void (*upstream_report_t)(int);

class RpmInstaller
{
public:
    //install groups in order
    RpmInstaller(const list<string> &groups, const string &rootdir);
    ~RpmInstaller();
    bool install(void (*progress)(int percent) = NULL);

    void reportUpstream(const string &rpm, int order, rpm_loff_t amount, rpm_loff_t total);

private:
    list<string> _groups;
    string _rootdir;
    rpmts _rpmts;
    string _groupPath; // when to find group config file in livecd

    list<string> _rpms; // to be installed
    upstream_report_t _reporter;

    bool setupRpm();
    /*
     * read all rpm filepaths from groups with order 
     */
    void prepareGroupRpms();
    /* 
     * check if group path exists and prepare for reading
     */
    bool sanitizeGroupPath();
    list<string> enumerateRpmsInGroupDir(const string &groupdir);
    void addRpmToTs(const string &rpmfile);
};

#endif

