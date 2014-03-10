#ifndef LIBINSTALLERBASE_ENGINE_INSTALLENGINE_H_
#define LIBINSTALLERBASE_ENGINE_INSTALLENGINE_H_

#include <glib.h>
#include <string>
#include <vector>
#include <list>

using namespace std;

static const string backup 		    = "1";
static const string no_backup 		= "0";
static const string no_self_test	= "0";
static const string self_test_root	= "1";
static const string self_test		= "2";
static const string fstab 		    = "fstab.bak";
static const string mount_para_devpts   = "gid=5, mode=620";
static const string mount_para_def 	    = "defaults";

struct fstab_struct
{
    string devpath;
    string mountpoint;
    string fstype;
    string mount_para;
    string backup;
    string self_test;
};

class Engine 
{
 public:
    // not write the conffile when in install mode.
    // not do real work when in write conffile, but write a xml file.
    // readconf is used by installer from console.
    enum WorkMode {Install, WriteConf, ReadConf};

    // the type of commands, install commands are invoked in installation progress, 
    // post commands are invoked in post-script.
    enum {INSTALL, POST};
    
    enum Stage {ADD, UPGRADE, CONFLICTS, DISKSPACE, INTEGRITY, LOAD, KEYRING};

    ~Engine();
    
    // At first time, please DO NOT call instance() with default args.
    static Engine* instance(WorkMode mode=WriteConf, const char *conf_file = NULL);
    
    // return false when fail, and error message in getErr()
    bool install(void (*progress)(Stage stage, int percent) =NULL);
    bool postscript(void);
    const char *getErr(void) { return _errstr.c_str(); }
    
    // cmd interfaces. 
    // the external program call these interface to order the commands.
    void cmdMakeLabel(const char *devpath, const char *labeltype);
    void cmdMakePartWhole(const char *devpath, const char *index, const char *parttype, const char *fstype);
    void cmdMakePartLength(const char *devpath, const char *index, const char *parttype, const char *fstype, const char *length);
    void cmdRemovePart(const char *partpath);
    void cmdRemoveAllPart(const char *devpath);
    void cmdMakeFileSystem(const char *partpath, const char *fstype);
    void cmdSetMountPoint(const char *devpath, const char *mountpoint, const char *fstype);
    void cmdChooseGroups(const char *groups);
    void cmdAddPackage(const char *package);
    void cmdAddGroup(const char *group);
    void cmdSetBootEntry(const char *todevpath);
    void cmdSetKernelParam(const char* param);
    void cmdSetLang(const char *locale);
    void cmdAddUser(const char *username, const char * = NULL);
    void cmdSetRootPassword(const char *passwd);
    void cmdSetKL(const char *kxkb);
    void cmdSetTZ(const char *kzoneinfo);
    void cmdSetXorgConf(const char* xorgconf);
    void cmdSetGlobals(const char* kglobals);
    void cmdSetHostname(const char* hostname);
    void cmdSetMachine(const char* mach_type);

 protected:    
    Engine(WorkMode mode, const char *conf_file);
    
 private:
    static void xmlStartElement(GMarkupParseContext *context, const gchar *element_name,
                                const gchar **attribute_names,	const gchar **attribute_values,
                                gpointer user_data, GError **error);
    static void xmlEndElement(GMarkupParseContext *context, const gchar *element_name,
                              gpointer user_data, GError **error);
    static void xmlText(GMarkupParseContext *context, const gchar *text,
                        gsize text_len, gpointer user_data, GError **error);
    static void xmlError(GMarkupParseContext *context, GError *error, gpointer user_data);

    bool realWork(void (*progress)(Stage, int));
    bool readConf(const char *conf_file);
    
    static Engine *s_self;
    string _errstr;
    WorkMode _workmode;

    // temp funcs, remove them when refactory is done.
    /*
    void copyKxkbrc();
    void copyKtimezonedrc();
    void copyXorgConf();
    */
    // classify the xml command into static commands vector.
    void appendCmd(int type, int cmdid, const char *arg0=NULL, const char *arg1=NULL,
                   const char *arg2=NULL, const char *arg3=NULL, const char *arg4=NULL);
    // ids of commands
    enum {
        SET_PARTITION = 0,
        PARTITION_MKLABEL,
        PARTITION_MKPART_WHOLE,
        PARTITION_MKPART_LENGTH,
        PARTITION_RMPART,
        PARTITION_RMALLPART,
        PARTITION_MKFS,
        PARTITION_SET_MOUNTPOINT,
        CHOOSE_GROUPS, // choose rpm groups, this is for cetcos
        ADD_PACKAGE,
        ADD_GROUP,
        SET_BOOT_ENTRY,
        SET_KERNEL_PARAM,
        SET_LANG,
        ADD_USER,
        SET_ROOT,
        SET_KL,
        SET_TZ,
        SET_XORG_CONF,
        SET_KGLOBALS,
        SET_HOSTNAME,
        SET_MACHINE,
        LASTTAG
    };
    
    struct Cmd {
        int id;
        vector<string> args;
    };

    static vector<Cmd> s_installcmds;
    static vector<Cmd> s_postcmds;
    static const char* s_tags[LASTTAG];

    bool runCmd(const vector<Cmd> &cmds);
    bool do_mklabel(const string &devpath, const string &labeltype);
    bool do_mkpart_whole(const string &devpath, const string &index, const string &parttype, const string &fstype);
    bool do_mkpart_length(const string &devpath, const string &index, const string &parttype, const string &fstype, const string &length);
    bool do_rmpart(const string &partpath);
    bool do_rmallpart(const string &devpath);
    bool do_mkfs(const string &partpath, const string &fstype);
    bool do_set_mountpoint(const string &devpath, const string &mountpoint, const string &fstype);
    bool do_choosegroups(const string &groups);
    bool do_add_package(const string package);
    bool do_add_group(const string group);
    bool do_boot_install(const string &devpath);
    bool do_set_kernel_param(const string &param);
    bool do_set_lang(const string &locale);
    bool do_add_user(const string &username, const string &passwd);
    bool do_set_root_passwd(const string &passwd);
    bool do_set_kl(const string &kxkb);
    bool do_set_tz(const string &kzoneinfo);
    bool do_set_xorg_conf(const string &xorgconf);
    bool do_set_kglobals(const string &kglobals);
    bool do_set_hostname(const string &hostname);
    list<string> _postscript;    // postscript commands and args.

    bool copy_files(void (*progress)(int percent), double range);
    bool prepareFileSystem();
    bool doMountRoot();
    bool doSetupFstab();

    string _rootdir;
    string _rootdev;
    string _filename;

    list<fstab_struct> _fstablist;
    list<string> _package_list;
    list<string> _rpm_groups; // for cetcos
    string _boot_partition;
    string _locale;
    string _grub_install_device;
    list<string> _new_user_names;
};
#endif // LIBINSTALLERBASE_ENGINE_INSTALLENGINE_H_
