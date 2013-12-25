#include "rpminstaller.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <set>
#include <errno.h>
#include <rpm/rpmlog.h>

using namespace std;

static const string g_default_group_path = "/var/lib/cetcosinstaller/RPMS";
static const char *g_macro_dbpath = "_dbpath";
static const char *g_default_dbpath = "/var/lib/rpm";

void *_rpm_callback(const void * h, const rpmCallbackType what,
        const rpm_loff_t amount, const rpm_loff_t total, fnpyKey key,
        rpmCallbackData data)
{
    static int order = 0;
    string rpmname;
    static FD_t fd = NULL;
    Header hh = (Header)h;
    if (key) {
        rpmname = string((const char *)key);   
    } else if (h) {
        char *s = headerGetAsString(hh, RPMTAG_NEVR);
        rpmname = string(s);
        free(s);
    }

    RpmInstaller *installer = (RpmInstaller*)data;

    switch(what) {
        case RPMCALLBACK_TRANS_PROGRESS:
        case RPMCALLBACK_INST_PROGRESS: 
            {
                //float p =  (total ? ((float)amount / total) * 100 : 100.0);
                //cerr << rpmname << " " << p << "%" << endl; 
                break;
            }

        case RPMCALLBACK_INST_START:
            cerr << "installing " << rpmname << endl;
            installer->reportUpstream(rpmname, ++order, amount, total);
            break;

        case RPMCALLBACK_INST_OPEN_FILE:
            if (!key) {
                return NULL;
            }
            fd = Fopen((const char *)key, "r.ufdio");
            if (fd == NULL || Ferror(fd)) {
                cerr << "Fopen failed: " << Fstrerror(fd) << endl;
                if (fd != NULL) {
                    Fclose(fd);
                    fd = NULL;
                }
            } else 
                fd = fdLink(fd);
            return (void*)fd;
            break;

        case RPMCALLBACK_INST_CLOSE_FILE:
            fd = fdFree(fd);
            if (fd != NULL) {
                Fclose(fd);
                fd = NULL;
            }
            break; 

        case RPMCALLBACK_TRANS_START:
            cerr << "transaction start\n"; break;
        case RPMCALLBACK_TRANS_STOP:
            //installer->reportUpstream(100.0);
            cerr << "transaction finished\n"; break;

            //case RPMCALLBACK_UNINST_PROGRESS:
            //case RPMCALLBACK_UNINST_START:
            //case RPMCALLBACK_UNINST_STOP:
        case RPMCALLBACK_UNPACK_ERROR:
            cerr << "unpack error " << rpmname << endl; break;

        case RPMCALLBACK_CPIO_ERROR:
            if (key) {
                rpmname = string((const char *)key);   
            } else if (h) {
                char *s = headerGetAsString(hh, RPMTAG_NEVR);
                rpmname = string(s);
                free(s);
            }
            cerr << "cpio error " + rpmname + "\n"; break;

        case RPMCALLBACK_SCRIPT_ERROR:
            cerr << "script error " + rpmname + "\n"; break;

            //case RPMCALLBACK_SCRIPT_START:
            //case RPMCALLBACK_SCRIPT_STOP:
        //FIXME: rpm 4.9 does not contain this enum
        //case RPMCALLBACK_INST_STOP:
            //cerr << "install " << rpmname << "stop\n"; break;
                
        default: 
            break;
    }

    return NULL;
}


RpmInstaller::RpmInstaller(const list<string> &groups, const string &rootdir)
    :_groups(groups), _rootdir(rootdir), _rpmts(NULL), _nr_total_rpms(0), _reporter(NULL)
{
}

#define pkg_name(rpmname) ({ \
        string _pkgname(rpmname); \
        string::size_type idx = rpmname.rfind('-'); \
        if (idx != string::npos) { \
            idx = rpmname.rfind('-', idx-1); \
            if (idx != string::npos) { \
                _pkgname = rpmname.substr(0, idx);  \
            } \
        } \
        _pkgname; \
        })

static bool is_rpm(const string &rpmname)
{
    string::size_type idx = rpmname.rfind(".rpm");
    return idx != string::npos && (idx == rpmname.length() - 4);
}

static void _list_append(list<string> &l, const list<string> &l2)
{
    list<string>::const_iterator i = l2.begin();
    while (i != l2.end()) {
        l.push_back(*i);
        ++i;
    }
}

void RpmInstaller::collectRpmSize()
{
    list<string>::const_iterator i = _groups.begin();
    while (i != _groups.end()) {
        string groupdir = _groupPath + "/RPMS." + *i;
        struct stat statbuf;
        if (stat(groupdir.c_str(), &statbuf) < 0) {
            perror("stat");
            ++i;
            continue;
        }

        if (!S_ISDIR(statbuf.st_mode)) {
            ++i;
            continue;
        }

        list<string> l = enumerateRpmsInGroupDir(groupdir);
        _nr_total_rpms += l.size();
        ++i;
    }

    cerr << "_nr_total_rpms: " << _nr_total_rpms << endl;
}

/*
 * run transactions for each group in order
 *
 * since core and base may have the same package with different versions, so 
 * installation needs to handle this situation by install each group as different
 * transaction in order (core then base then extra etc).
 */
bool RpmInstaller::setupTransactions()
{
    //TODO: maybe find orders of groups by config file

    //seperate groups by transactions, groups need to be in order
    cerr << "totally " << _groups.size() << " groups to install\n";
    //sanity check
    if (_groups.front() != "core") {
        //need to be reordered
        cerr << "groups need to be reordered\n";
    }

    collectRpmSize();

    bool upgrade = false;
    list<string>::const_iterator i = _groups.begin();
    while (i != _groups.end()) {
        string groupdir = _groupPath + "/RPMS." + *i;
        struct stat statbuf;
        if (stat(groupdir.c_str(), &statbuf) < 0) {
            perror("stat");
            ++i;
            continue;
        }

        if (!S_ISDIR(statbuf.st_mode)) {
            ++i;
            continue;
        }

        cerr << "process group " << *i << endl;
        upgrade = (*i != "core");
        if (!buildAndRunForGroup(groupdir, upgrade))
            return false;
        ++i;
    }

    return true;
}

bool RpmInstaller::buildAndRunForGroup(const string &groupdir, bool upgrade)
{
    if (prepareTransaction(groupdir, upgrade)) {
        return runTransaction(); 
    }
    return false;
}

bool RpmInstaller::prepareTransaction(const string &groupdir, bool upgrade)
{
#ifdef DEBUG
    rpmlogSetMask(RPMLOG_MASK(RPMLOG_DEBUG));
#endif
    _rpmts = rpmtsCreate();
    rpmtsSetNotifyCallback(_rpmts, _rpm_callback, this);

    rpmtsSetRootDir(_rpmts, _rootdir.c_str());
    rpmInitMacros(rpmGlobalMacroContext, "/usr/lib/rpm/macros");
    delMacro(rpmGlobalMacroContext, g_macro_dbpath);
    addMacro(rpmGlobalMacroContext, g_macro_dbpath, NULL, g_default_dbpath, 0);

    char sbuf[512] = "%{_dbpath}"; 
    if (expandMacros(NULL, NULL, sbuf, sizeof sbuf - 1) != 0) {
        cerr << "_dbpath expand failed, need to set it\n";
        addMacro(NULL, g_macro_dbpath, NULL, g_default_dbpath, 0);
    }

    cerr << "_dbpath " << sbuf << endl;
    //rpmDumpMacroTable(NULL, stderr);

    rpmtsSetDBMode(_rpmts, O_RDWR);

    _rpms.clear();
    _list_append(_rpms, enumerateRpmsInGroupDir(groupdir));
    list<string>::const_iterator i = _rpms.begin();
    while (i != _rpms.end()) {
        addRpmToTs(_rpmts, *i, upgrade);
        ++i;
    }

    return true;
}

bool RpmInstaller::runTransaction()
{
    rpmps ps = NULL;
    string err;

    //rpmtsSetFlags(_rpmts, RPMTRANS_FLAG_NOMD5|RPMTRANS_FLAG_NOFILEDIGEST);
    //rpmtsSetVSFlags(_rpmts, _RPMVSF_NODIGESTS |_RPMVSF_NOSIGNATURES);
    int rc = rpmtsOrder(_rpmts);
    if (rc) {
        err = "rpms can not be ordered";
        goto _label_error;
    }

    ps = rpmtsProblems(_rpmts);
    if ((rc = rpmpsNumProblems(ps)) > 0) {
        err = "has problems";
        goto _label_error;
    }
    rpmpsFree(ps);

    cerr << "begin to run transaction\n";
    rc = rpmtsRun(_rpmts, NULL, RPMPROB_FILTER_IGNOREARCH|RPMPROB_FILTER_IGNOREOS);
    cerr << "transaction done with " << rc << endl;

    ps = rpmtsProblems(_rpmts);
    if ((rc = rpmpsNumProblems(ps)) > 0) {
        err = "has problems";
        goto _label_error;
    }

_label_error:
    if (rc) {
        cerr << err << endl;
        rpmpsPrint(stderr, ps);
    }
    rpmpsFree(ps);
    rpmtsFree(_rpmts);
    return rc == 0;
}

bool RpmInstaller::preprocess()
{
    cerr << __PRETTY_FUNCTION__ << endl;
    if (!sanitizeGroupPath()) {
        return false;
    }
    return processPrivileged();

}

bool RpmInstaller::processPrivileged()
{
#ifdef DEBUG
    rpmlogSetMask(RPMLOG_MASK(RPMLOG_DEBUG));
#endif

    rpmts ts = rpmtsCreate();
    rpmtsSetNotifyCallback(ts, _rpm_callback, this);

    //rpmtsSetFlags(ts, RPMTRANS_FLAG_NOMD5|RPMTRANS_FLAG_NOFILEDIGEST);
    //rpmtsSetVSFlags(ts, _RPMVSF_NODIGESTS |_RPMVSF_NOSIGNATURES);

    rpmtsSetRootDir(ts, _rootdir.c_str());
    rpmInitMacros(rpmGlobalMacroContext, "/usr/lib/rpm/macros");
    delMacro(rpmGlobalMacroContext, g_macro_dbpath);
    addMacro(rpmGlobalMacroContext, g_macro_dbpath, NULL, g_default_dbpath, 0);

    char sbuf[512] = "%{_dbpath}"; 
    if (expandMacros(NULL, NULL, sbuf, sizeof sbuf - 1) != 0) {
        cerr << "_dbpath expand failed, need to set it\n";
        addMacro(NULL, g_macro_dbpath, NULL, g_default_dbpath, 0);
    }

    cerr << "_dbpath " << sbuf << endl;
    //rpmDumpMacroTable(NULL, stderr);

    rpmtsSetDBMode(ts, O_RDWR);
    rpmtsInitDB(ts, O_RDWR);

    _privilegedRpms.insert("setup");
    _privilegedRpms.insert("filesystem");

    list<string> rpms;

    string core_group_dir = _groupPath + "/RPMS.core";
    DIR *dir = opendir(core_group_dir.c_str());
    if (!dir) {
        perror("opendir");
        return false;
    }

    errno = 0;
    cerr << "scanning " << core_group_dir << endl;
    int count = 0;
    struct dirent *entry = NULL;
    do {
        entry = readdir(dir);
        count++;
        if (entry) {
            string fname(entry->d_name);
            cerr << "processing " << fname << endl;

#ifdef _DIRENT_HAVE_D_TYPE 
            if (entry->d_type != DT_UNKNOWN) {
                if (entry->d_type != DT_REG)
                    continue;
            } else {
                cerr << "d_type is unknown, check name only\n";
            }
#endif
            if (!is_rpm(fname)) continue;

            if (_privilegedRpms.find(pkg_name(fname)) != _privilegedRpms.end()) {
                rpms.push_back(core_group_dir + "/" + fname);
                _escapedRpms.insert(fname);
                cerr << "add escape rpm" << fname << endl;
            }

            if (rpms.size() == _privilegedRpms.size()) 
                break; //search done
        }
    } while (entry);

    cerr << "readdir return NULL with count " << count << endl;
    if (errno) {
        perror("readdir");
    }
    closedir(dir);

    list<string>::const_iterator i = rpms.begin();
    while (i != rpms.end()) {
        addRpmToTs(ts, *i);
        ++i;
    }

    int rc = rpmtsRun(ts, NULL, RPMPROB_FILTER_IGNOREARCH|RPMPROB_FILTER_IGNOREOS);
    rpmtsFree(ts);
    cerr << "install privileged done with " << rc << endl;

    // do dirty work
    // create necessary dev node for postscripts

    setupChroot();
    return true;
}

bool RpmInstaller::setupChroot()
{
    char buf[1024];
    snprintf(buf, sizeof buf - 1,
            "mkdir %s/dev && cd %s/dev && mknod null c 1 3 "
            " && mknod console c 5 1 && cd /" 
            " && mount --bind /dev %s/dev "
            " && mount --bind /sys %s/sys"
            " && mount --bind /proc %s/proc", 
            _rootdir.c_str(), _rootdir.c_str(), _rootdir.c_str(), 
            _rootdir.c_str(), _rootdir.c_str());
    cerr << "setupChroot: [" << buf << "]\n";
    if (system(buf) < 0) {
        cerr << "setupChroot failed\n";
        return false;
    }

    return true;
}

bool RpmInstaller::install(void (*progress)(int percent))
{

    _reporter = progress;
    setupTransactions();
    if (_reporter)
        _reporter(100);
    return true;
}

RpmInstaller::~RpmInstaller()
{
}

list<string> RpmInstaller::enumerateRpmsInGroupDir(const string &groupdir)
{
    list<string> l;
    DIR *dir = opendir(groupdir.c_str());
    if (!dir) {
        perror("opendir");
        return l;
    }

    struct dirent *entry = NULL;
    do {
        entry = readdir(dir);
        if (entry) {
#ifdef _DIRENT_HAVE_D_TYPE 
            if (entry->d_type != DT_UNKNOWN) {
                if (entry->d_type != DT_REG)
                    continue;
            } else {
                //cerr << "d_type is unknown, check name only\n";
            }
#endif

            string fname(entry->d_name);
            if (!is_rpm(fname)) continue;

            if (fname.size() && _escapedRpms.find(fname) == _escapedRpms.end()) {
                l.push_back(groupdir + "/" + fname);
                //cerr << "add " << l.back() << endl;
            }
        }
    } while (entry);
    closedir(dir);
    return l;
}

//TODO: needs to be done when ISO prepared
bool RpmInstaller::sanitizeGroupPath()
{
    _groupPath = g_default_group_path;
    return true;
}

void RpmInstaller::addRpmToTs(rpmts ts, const string &rpmfile, bool upgrade)
{
    cerr << "addRpmToTs " << rpmfile << endl;
    FD_t fd = Fopen(rpmfile.c_str(), "r.ufdio");
    Header h;
    int rc;
    rpmRC ret;

    if (!fd) {
        cerr << "Fopen failed for" << rpmfile << endl;
        goto _finished;
    }

    ret = rpmReadPackageFile(ts, fd, rpmfile.c_str(), &h);
    if (ret != RPMRC_OK) {
        cerr << "rpmReadPackageFile failed\n";
        goto _finished;
    }

    rc = rpmtsAddInstallElement(ts, h, rpmfile.c_str(), upgrade?1:0, NULL);
    if (rc) {
        cerr << "rpmtsAddInstallElement failed with" << rc << "\n";
        goto _finished;
    }

_finished:
    Fclose(fd);
}

void RpmInstaller::reportUpstream(const string &rpm, int order, rpm_loff_t amount, rpm_loff_t total)
{
    if (!_reporter) return;

    int nr_elems = rpmtsNElements(_rpmts);
    nr_elems = _nr_total_rpms;
    float p =  (nr_elems ? ((float)order / nr_elems) * 100 : 100.0);
    _reporter(p);
}
