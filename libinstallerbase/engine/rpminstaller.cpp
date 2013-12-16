#include "rpminstaller.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <rpm/rpmlog.h>

using namespace std;

static const string g_default_group_path = "/var/lib/cetcosinstaller";
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
                float p =  (total ? ((float)amount / total) * 100 : 100.0);
                cerr << rpmname << " " << p << "%" << endl; 
                //installer->reportUpstream(100.0);
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
        case RPMCALLBACK_INST_STOP:
            cerr << "install " << rpmname << "stop\n"; break;
                
        default: 
            break;
    }

    return NULL;
}


RpmInstaller::RpmInstaller(const list<string> &groups, const string &rootdir)
    :_groups(groups), _rootdir(rootdir), _rpmts(NULL)
{
    setupRpm();
}

bool RpmInstaller::install(void (*progress)(int percent))
{
    _reporter = progress;
    rpmps ps;

    //rpmtsSetFlags(_rpmts, RPMTRANS_FLAG_BUILD_PROBS);
    int rc = rpmtsOrder(_rpmts);
    if (rc) {
        cerr << rc << " rpms can not be ordered\n";
        return false;
    }

    ps = rpmtsProblems(_rpmts);
    if (rpmpsNumProblems(ps) > 0) {
        cerr << "has problems\n";
        rpmpsFree(ps);
        return false;
    }
    rpmpsFree(ps);

    cerr << "begin to run transaction\n";
    rc = rpmtsRun(_rpmts, NULL, RPMPROB_FILTER_IGNOREARCH|RPMPROB_FILTER_IGNOREOS);
    cerr << "transaction run done with " << rc << endl;

    _reporter(100);
    return true;
}

bool RpmInstaller::setupRpm()
{
    if (!sanitizeGroupPath()) {
        return false;
    }

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
    rpmtsInitDB(_rpmts, O_RDWR);

    prepareGroupRpms();

    list<string>::const_iterator i = _rpms.begin();
    while (i != _rpms.end()) {
        addRpmToTs(*i);
        ++i;
    }

    return true;
}

RpmInstaller::~RpmInstaller()
{
    if (_rpmts)
        rpmtsFree(_rpmts);
}

static void _list_append(list<string> &l, const list<string> &l2)
{
    list<string>::const_iterator i = l2.begin();
    while (i != l2.end()) {
        l.push_back(*i);
        ++i;
    }
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
            if (entry->d_type != DT_REG)
                continue;

            string fname(entry->d_name);
            if (fname.size()) {
                l.push_back(groupdir + "/" + fname);
                //cerr << "add " << l.back() << endl;
            }
        }
    } while (entry);
    closedir(dir);
    return l;
}

void RpmInstaller::prepareGroupRpms()
{
    //TODO: maybe find orders of groups by config file

    //seperate groups by transaction or not?

    cerr << "totally " << _groups.size() << " groups to install\n";
    list<string>::const_iterator i = _groups.begin();
    while (i != _groups.end()) {
        string groupdir = _groupPath + "/" + *i;
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
        _list_append(_rpms, enumerateRpmsInGroupDir(groupdir));
        ++i;
    }
}

//TODO: needs to be done when ISO prepared
bool RpmInstaller::sanitizeGroupPath()
{
    _groupPath = g_default_group_path;
    return true;
}

void RpmInstaller::addRpmToTs(const string &rpmfile)
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

    ret = rpmReadPackageFile(_rpmts, fd, rpmfile.c_str(), &h);
    if (ret != RPMRC_OK) {
        cerr << "rpmReadPackageFile failed\n";
        goto _finished;
    }

    rc = rpmtsAddInstallElement(_rpmts, h, rpmfile.c_str(), 1, NULL);
    if (rc) {
        cerr << "rpmtsAddInstallElement failed with" << rc << "\n";
        goto _finished;
    }

_finished:
    Fclose(fd);
}

void RpmInstaller::reportUpstream(const string &rpm, int order, rpm_loff_t amount, rpm_loff_t total)
{
    int nr_elems = rpmtsNElements(_rpmts);
    float p =  (nr_elems ? ((float)order / nr_elems) * 100 : 100.0);
    //float p =  (total ? ((float)amount / total) * 100 : 100.0);
    _reporter(p);
}
