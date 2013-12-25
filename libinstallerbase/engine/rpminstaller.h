#ifndef _RPM_INSTALLER_H
#define _RPM_INSTALLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
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
    bool preprocess();
    bool setupTransactions();
    int totalRpmSize() const { return _nr_total_rpms; }

    void reportUpstream(const string &rpm, int order, rpm_loff_t amount, rpm_loff_t total);

private:
    list<string> _groups;
    string _rootdir;
    rpmts _rpmts;
    string _groupPath; // when to find group config file in livecd

    list<string> _rpms; // to be installed in a group
    set<string> _privilegedRpms;  // that need to be installed at very first
    set<string> _escapedRpms; // that won't be installed (may be privileged which already installed at the first)
    int _nr_total_rpms; // No of rpms to install of all groups
    upstream_report_t _reporter;

    bool buildAndRunForGroup(const string &groupdir, bool upgrade = false);
    bool prepareTransaction(const string &groupdir, bool upgrade = false);
    bool runTransaction();

    void collectRpmSize();
    /* 
     * check if group path exists and prepare for reading
     */
    bool sanitizeGroupPath();
    list<string> enumerateRpmsInGroupDir(const string &groupdir);
    void addRpmToTs(rpmts ts, const string &rpmfile, bool upgrade = false);
    bool processPrivileged();
    bool setupChroot();
};

#endif

