#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iomanip>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <ctype.h>
#include <algorithm>

#include <parted/parted.h>
#include "parted++.h"
#include "gen_grub_conf.h"
#include "os_identify.h"
#include "alpminstaller.h"
#include "installengine.h"

#define DEBUG_MODE        
#ifdef   DEBUG_MODE  
  #define   debuglog     printf  
#else  
  #define   debuglog
#endif

using namespace std;

const char *debug_file = "/tmp/cetcosinstaller_debug.txt";
const char *default_conf_xml = "/tmp/isoft_conf.xml";

Engine *Engine::s_self = NULL;
extern const char *conf_tag[];
vector<Engine::Cmd> Engine::s_installcmds;
vector<Engine::Cmd> Engine::s_postcmds;
const char* Engine::s_tags[LASTTAG];

Engine* Engine::instance(WorkMode mode, const char *conf_file)
{
    if(s_self == NULL) {
        if((mode == WriteConf || mode == Install) && conf_file == NULL){
            s_self = new Engine(mode, default_conf_xml);
        }else{
            s_self = new Engine(mode, conf_file);
        }
    }
    return s_self;
}

Engine::Engine(WorkMode mode, const char *conf_file)
    : _workmode(mode), _errstr(""), _rootdir("")
{
    assert(!(mode == Install && conf_file == NULL));
    assert(!(mode == WriteConf && conf_file == NULL));
    assert(!(mode == ReadConf && conf_file == NULL));
    
    s_tags[SET_PARTITION]            = "SET_PARTITION";
    s_tags[PARTITION_MKLABEL]        = "PARTITION_MKLABEL";
    s_tags[PARTITION_MKPART_WHOLE]   = "PARTITION_MKPART_WHOLE";
    s_tags[PARTITION_MKPART_LENGTH]  = "PARTITION_MKPART_LENGTH";
    s_tags[PARTITION_RMPART]         = "PARTITION_RMPART";
    s_tags[PARTITION_RMALLPART]      = "PARTITION_RMALLPART";
    s_tags[PARTITION_MKFS]           = "PARTITION_MKFS";
    s_tags[PARTITION_SET_MOUNTPOINT] = "PARTITION_SET_MOUNTPOINT";
    s_tags[CHOOSE_GROUPS]            = "CHOOSE_GROUPS";
    s_tags[ADD_PACKAGE]              = "ADD_PACKAGE";
    s_tags[ADD_GROUP]                = "ADD_GROUP";
    s_tags[SET_BOOT_ENTRY]           = "SET_BOOT_ENTRY";
    s_tags[SET_KERNEL_PARAM]         = "SET_KERNEL_PARAM";
    s_tags[SET_LANG]                 = "SET_LANG";
    s_tags[ADD_USER]                 = "ADD_USER";
    s_tags[SET_ROOT]                 = "SET_ROOT";
    s_tags[SET_KL]                   = "SET_KL";
    s_tags[SET_TZ]                   = "SET_TZ";
    s_tags[SET_XORG_CONF]            = "SET_XORG_CONF";
    s_tags[SET_KGLOBALS]             = "SET_KGLOBALS";
    s_tags[SET_HOSTNAME]             = "SET_HOSTNAME";
    s_tags[SET_MACHINE]              = "SET_MACHINE";
    
    if(conf_file){
        _filename = conf_file;
    }
    
    if (_workmode == ReadConf) {
        readConf(_filename.c_str());
    }else if((_workmode == WriteConf) || (_workmode == Install)) {
        ofstream out(_filename.c_str(), ios::trunc); // empty the file
    }
    
    ofstream out(debug_file, ios::trunc);
    out.close();
}

Engine::~Engine()
{
    string target;

    // umount
    list<fstab_struct> ::reverse_iterator fstab_it;
    for (fstab_it = _fstablist.rbegin(); fstab_it != _fstablist.rend(); ++fstab_it){
        if (fstab_it->devpath.length() == 0)
            continue; 
        target = _rootdir + fstab_it->mountpoint;
        string umount = "umount ";
        umount += target;
        umount += " 2>> /tmp/install.log";
        cerr << umount << endl;
        system(umount.c_str());
    }
}

void Engine::xmlStartElement(GMarkupParseContext *context,
			     const gchar *element_name,					
			     const gchar **attribute_names,					
			     const gchar **attribute_values,
			     gpointer user_data,
			     GError **error)
{
    debuglog("<%s>", element_name);
    Cmd cmd;
    // get id
    for(int i = SET_PARTITION; i < LASTTAG; ++i) {
        if(string(s_tags[i]) == element_name) {
            cmd.id = i;
            break;
        }
    }

    // push args
    vector<string> args;
    for(int i = 0; i < 10; ++i) {
        if(attribute_names[i] != NULL && attribute_values[i] != NULL) {
            debuglog("%s = %s,", attribute_names[i], attribute_values[i]);
            args.push_back(attribute_values[i]);
        }else{
            break;
        }
    }
    cmd.args = args;
    s_installcmds.push_back(cmd);
    
    debuglog(">");
}

void Engine::xmlEndElement(GMarkupParseContext *context,
			   const gchar *element_name,
			   gpointer user_data,
			   GError **error)
{
    debuglog("</%s>\n", element_name);
}

void Engine::xmlText(GMarkupParseContext *context,
			const gchar *text,
			gsize text_len,
			gpointer user_data,
			GError **error
			)
{
    debuglog("%s", text);
}

void Engine::xmlError(	GMarkupParseContext *context,
			GError *error,
			gpointer user_data
			)
{
    debuglog("xml_error");
}

bool Engine::readConf(const char *conf_file)
{
    int fd;
    char *xml;
    int reval = 0;
    struct stat sbuf;
    GMarkupParser parser;
    GError *error;
    GMarkupParseContext *context;
	
    if (lstat(conf_file, &sbuf) < 0) {
        _errstr = "Can not Find conf file ";
        _errstr += conf_file;
        _errstr += "\n";
        return false;
    }
	
    if ((fd = open(conf_file, O_RDONLY)) == -1)	{
        _errstr = "open conf_file error\n";
        return false;
    }

    xml = (char *)mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (xml == MAP_FAILED) {
        _errstr = "mmap error\n";
        close(fd);
        return false;
    }

    parser.start_element = xmlStartElement;
    parser.end_element = xmlEndElement;
    parser.text = xmlText;
    parser.error = xmlError;
    parser.passthrough = NULL;

    context = g_markup_parse_context_new(&parser, G_MARKUP_DO_NOT_USE_THIS_UNSUPPORTED_FLAG,
                                         NULL, NULL);
	
    reval = g_markup_parse_context_parse(context, xml, sbuf.st_size, &error);
		
    g_markup_parse_context_end_parse(context, &error);
    g_markup_parse_context_free(context);
    munmap(xml, sbuf.st_size);
    close(fd);

    return reval;
}

bool Engine::install(void (*progress)(Engine::Stage, int))
{
    bool ret = false;
    int pkgpos = -1;
    bool haspkg = false;
    vector<Cmd> tmpcmds;

    // find post cmd and mv it    
    for(int i = s_installcmds.size() - 1; i >= 0; --i) {
        if(s_installcmds[i].id > ADD_GROUP) {
            // a post cmd, mv to postcmds
            s_postcmds.push_back(s_installcmds[i]);
            s_installcmds.erase(s_installcmds.begin()+i);
        }
    }
    
    // resort cmds so that run all partition cmd first(in original order)
    // then set_package(install), then other set_*
    for(int i = 0; i < s_installcmds.size(); ++i) {
        if(s_installcmds[i].id > ADD_PACKAGE && !haspkg ) {
            tmpcmds.push_back(s_installcmds[i]);
            vector<Cmd>::iterator it = s_installcmds.begin()+i;
            s_installcmds.erase(it);
            --i;
        }else if(s_installcmds[i].id == ADD_PACKAGE) {
            haspkg = true;
            pkgpos = i;
            for(int j = 0; j < tmpcmds.size(); ++j) {
                s_installcmds.push_back(tmpcmds[j]);
                tmpcmds.clear();
            }
            break;
        }
    }

    if(pkgpos == -1) {
        for(int j = 0; j < tmpcmds.size(); ++j) {
            s_installcmds.push_back(tmpcmds[j]);
        }
        tmpcmds.clear();
    }
    
    // real install
    if (_workmode == WriteConf) {
        if(progress) {
            (*progress)(Engine::ADD, 100);
        }
        ret = true;
    }else{
        ret = realWork(progress);
    }
    
    return ret;
}

bool Engine::postscript(void)
{
    int ret = 0;

    // run post cmds
    if( _workmode == Install || _workmode == ReadConf )	{
		if(!runCmd(s_postcmds)) {
			_errstr = "run postcmds error";
			return false;
		}

		string cmd;

		// create the postscript.sh from template, append cmds from _postscript.
		string post = _rootdir+"postscript.sh";
        {
            //PRESTAGE
            ofstream script(post.c_str(), ios::out);
            script << "#!/bin/bash" << endl;
            script << "set -x" << endl;
            script << "export LC_ALL=" + _locale << endl;
            script << "export LANG=" + _locale << endl;
            script << "export GRUB_DEVICE=" << _grub_install_device << endl;
            list<string>::const_iterator ci = _new_user_names.begin();
            int i = 1;
            while (ci != _new_user_names.end()) {
                script << "export USER_NAME" << i++ << "=" << *ci << endl;
                ++ci;
            }

            // trans postscript to real script.
            for(list<string>::iterator it = _postscript.begin(); it != _postscript.end(); ++it) {
                script << *it << endl;
                cerr << "append [" << *it << "]\n";
            }

            script.close();
        }

		cmd = "cat /usr/share/apps/libinstallerbase/postscript.tmpl >> " + post;
		system(cmd.c_str());
		chmod(post.c_str(), 0755);

        {
            //POSTSTAGE
            ofstream script(post.c_str(), ios::app);
            if(!script) {
                _errstr = "open postscript.sh file failederror";
                return false;
            }

            script << endl;
            script << "echo postscript done" << endl;
            script.close();
        }
    
		// run postscript
		cmd = "chroot ";
		cmd += _rootdir;
		cmd += " /postscript.sh &> ";
		cmd += _rootdir;
		cmd += "tmp/postscript.log";
		if ( system( cmd.c_str()) != 0 ) {
			debuglog("run cmd %s FAILED.\n", cmd.c_str());
		}

         //delete postscript
        //cmd = "rm -rf " + _rootdir + "postscript.sh";
        //if ( system(cmd.c_str()) != 0) {
            //debuglog("delete postscript failed: %s\n", cmd.c_str() );
        //}

        // umount /dev /sys /proc
        system(("umount " + _rootdir + "/dev").c_str());
        system(("umount " + _rootdir + "/sys/fs/fuse/connections").c_str());
        system(("umount " + _rootdir + "/sys").c_str());
        system(("umount " + _rootdir + "/proc").c_str());

    }// run post cmds end
    
    // write to configure file
    if(_workmode == Install || _workmode == WriteConf) {
        ofstream out(_filename.c_str(), ios::app);
        if(out) {
            Cmd cmd;
            for(int i = 0; i < s_installcmds.size(); ++i){
                cmd = s_installcmds[i];
                out << "<" << s_tags[cmd.id] << " ";
                for(int j = 0; j < cmd.args.size(); ++j) {
                    out << "ARG='" << cmd.args[j] << "' ";
                }
                out << "></" << s_tags[cmd.id] << ">" << endl;
            }
            
            for(int i = 0; i < s_postcmds.size(); ++i){
                cmd = s_postcmds[i];
                out << "<" << s_tags[cmd.id] << " ";
                for(int j = 0; j < cmd.args.size(); ++j) {
                    out << "ARG='" << cmd.args[j] << "' ";
                }
                out << "></" << s_tags[cmd.id] << ">" << endl;
            }

        }else{
            _errstr = "open configure file for write failed.";
            ret = -1;
        }
    }

    if( _workmode == Install || _workmode == ReadConf )	
	{
    	//XXX copy .xml file to installed system
		string cmd_copy = string("/bin/cp ") + default_conf_xml;
		cmd_copy += " " + _rootdir + "root/";
		ret = system(cmd_copy.c_str());
	}

    return (ret == 0) ? true : false;
}

bool Engine::runCmd(const vector<Cmd> &cmds)
{
    bool ret = true;

    FILE *fp = NULL;

    if ((fp = popen("swapoff -a", "r")) == NULL) {
        debuglog("swapoff -a error\n");
        return false;
    }

    pclose(fp);

    for(int i = 0; i < cmds.size(); ++i) {
        const Cmd currcmd = cmds[i];
        switch(currcmd.id){
        case PARTITION_MKLABEL:
            ret = do_mklabel(currcmd.args[0],
                             currcmd.args[1]);
            if(!ret){
                _errstr = "Run mklabel Failed.";
                return false;
            }
            break;
        case PARTITION_MKPART_WHOLE:
            ret = do_mkpart_whole(currcmd.args[0],
                                  currcmd.args[1],
                                  currcmd.args[2],
                                  currcmd.args[3]);
            if(!ret){
                _errstr = "Run mkpart_whole Failed.";
                return false;
            }
            break;
        case PARTITION_MKPART_LENGTH:
            ret = do_mkpart_length(currcmd.args[0],
                                   currcmd.args[1],
                                   currcmd.args[2],
                                   currcmd.args[3],
                                   currcmd.args[4]);
            if(!ret){
                _errstr = "Run mkpart_length Failed.";
                return false;
            }
            break;
        case PARTITION_RMPART:
            ret = do_rmpart(currcmd.args[0]);
            if(!ret){
                _errstr = "Run rmpart Failed.";
                return false;
            }
            break;
        case PARTITION_RMALLPART:
            ret = do_rmallpart(currcmd.args[0]);
            if(!ret){
                _errstr = "Run rmallpart Failed.";
                return false;
            }
            break;
        case PARTITION_MKFS:
            ret = do_mkfs(currcmd.args[0], currcmd.args[1]);
            if(!ret){
                _errstr = "Run mkfs Failed.";
                return false;
            }
            break;
        case PARTITION_SET_MOUNTPOINT:
            ret = do_set_mountpoint(currcmd.args[0], currcmd.args[1], currcmd.args[2]);
            if(!ret){
                _errstr = "Run mountpoint Failed.";
                return false;
            }
            break;
        case CHOOSE_GROUPS:
            ret = do_choosegroups(currcmd.args[0]);
            if(!ret){
                _errstr = "Run choosegroups Failed.";
                return false;
            }
            break;
        case ADD_PACKAGE:
            if(_rootdir == ""){
                _errstr = "Run set_package Failed, no rootdir to install";
                return false;
            }
            ret = do_add_package(currcmd.args[0]);
            if(!ret){
                _errstr = "Run add_package Failed.";
                return false;
            }
            break;
        case ADD_GROUP:
            if(_rootdir == ""){
                _errstr = "Run add_group Failed";
                return false;
            }
            ret = do_add_group(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_package Failed.";
                return false;
            }
            break;
        case SET_BOOT_ENTRY:
            ret = do_boot_install(currcmd.args[0]);
            if(!ret){
                _errstr = "Run boot_install Failed.";
                return false;
            }
            break;
        case SET_KERNEL_PARAM:
            ret = do_set_kernel_param(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set kernel param Failed.";
                return false;
            }
            break;
        case SET_LANG:
            ret = do_set_lang(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_lang Failed.";
                return false;
            }
            break;
        case ADD_USER:
            ret = do_add_user(currcmd.args[0], currcmd.args[1]);
            if(!ret){
                _errstr = "Run add_user Failed.";
                return false;
            }
            break;
        case SET_ROOT:
            ret = do_set_root_passwd(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_root_passwd Failed.";
                return false;
            }
            break;
        case SET_KL:
            ret = do_set_kl(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_kl Failed.";
                return false;
            }
            break;
        case SET_TZ:
            ret = do_set_tz(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_tz Failed.";
                return false;
            }
            break;
        case SET_XORG_CONF:
            ret = do_set_xorg_conf(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_xorg_conf Failed.";
                return false;
            }
            break;
        case SET_KGLOBALS:
            ret = do_set_kglobals(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_kglobals Failed.";
                return false;
            }
            break;
        case SET_HOSTNAME:
            ret = do_set_hostname(currcmd.args[0]);
            if(!ret){
                _errstr = "Run set_hostname Failed.";
                return false;
            }
            break;
        case SET_MACHINE:
            //TODO: do what?
            ret = true;
            break;
        default:
            ;
        }
    }
    
    return ret;
}

bool Engine::realWork(void (*progress)(Engine::Stage, int))
{
    static int percent = 0;
    // output debug log
    ofstream out(debug_file, ios::app);
    for(int i = 0; i < s_installcmds.size(); ++i){
        const Cmd currcmd = s_installcmds[i];
        out << s_tags[currcmd.id] << "(" << currcmd.id << ") ";
        for(int j = 0; j < currcmd.args.size(); ++j){
            out << "ARG" << "=" << currcmd.args[j].c_str() << ", ";
        }
        out << endl;
    }
  
    // for installer debug
    if (getenv("RF_REAL_INSTALL") != NULL) {
        return true;
    }

    // umount all partitions.
    //system("awk '/^\\/dev\\/sd[abcd]/ {print $1}' /etc/mtab | xargs umount");
    
    // run commands from XML.
    if(!runCmd(s_installcmds)){
        out << "runCmd return false" << endl;
        out << _errstr << endl;
        cerr << "runCmd with err: " << _errstr << endl;
        return false;
    }

    // prepare to install
    out << "prepare to install" << endl;    

    doMountRoot();
    AlpmInstaller grpInstaller(_rpm_groups, _rootdir);
    grpInstaller.preprocess();
    bool ret = grpInstaller.install(progress);
    doSetupFstab();

    if(!ret){
        _errstr = "install failed.";
        return false;
    }
    
    progress(Engine::ADD, 100);

    return true;
}

void Engine::cmdMakeLabel(const char *devpath, const char *labeltype)
{
    appendCmd(INSTALL, PARTITION_MKLABEL, devpath, labeltype);
}

void Engine::cmdMakePartWhole(const char *devpath, const char *index, const char *parttype, const char *fstype)
{
    appendCmd(INSTALL, PARTITION_MKPART_WHOLE, devpath, index, parttype, fstype);
}
void Engine::cmdMakePartLength(const char *devpath, const char *index, const char *parttype,
                            const char *fstype, const char *length)
{
    appendCmd(INSTALL, PARTITION_MKPART_LENGTH, devpath, index, parttype, fstype, length);
}
void Engine::cmdRemovePart(const char *partpath)
{
    appendCmd(INSTALL, PARTITION_RMPART, partpath);
}
void Engine::cmdRemoveAllPart(const char *devpath)
{
    appendCmd(INSTALL, PARTITION_RMALLPART, devpath);
}
void Engine::cmdMakeFileSystem(const char *partpath, const char *fstype)
{
    appendCmd(INSTALL, PARTITION_MKFS, partpath, fstype);
}
void Engine::cmdSetMountPoint(const char *devpath, const char *mountpoint, const char *fstype)
{
    appendCmd(INSTALL, PARTITION_SET_MOUNTPOINT, devpath, mountpoint, fstype);
}
void Engine::cmdChooseGroups(const char *groups)
{
    appendCmd(INSTALL, CHOOSE_GROUPS, groups);
}
void Engine::cmdAddPackage(const char *package)
{
    appendCmd(INSTALL, ADD_PACKAGE, package);
}
void Engine::cmdAddGroup(const char *group)
{
    appendCmd(INSTALL, ADD_GROUP, group);
}
void Engine::cmdSetBootEntry(const char *todevpath)
{
    appendCmd(POST, SET_BOOT_ENTRY, todevpath);
}
void Engine::cmdSetKernelParam(const char *param)
{
    appendCmd(POST, SET_KERNEL_PARAM, param);
}
void Engine::cmdSetLang(const char *locale)
{
    appendCmd(POST, SET_LANG, locale);
}
void Engine::cmdAddUser(const char *username, const char *passwd)
{
    if (passwd == NULL)
        appendCmd(POST, ADD_USER, username, "");
    else
        appendCmd(POST, ADD_USER, username, passwd);
}
void Engine::cmdSetRootPassword(const char *passwd)
{
    appendCmd(POST, SET_ROOT, passwd);
}

void Engine::cmdSetKL(const char *kxkb)
{
    appendCmd(POST, SET_KL, kxkb);
}

void Engine::cmdSetTZ(const char *kzoneinfo)
{
    appendCmd(POST, SET_TZ, kzoneinfo);
}

void Engine::cmdSetXorgConf(const char *xorgconf)
{
    appendCmd(POST, SET_XORG_CONF, xorgconf);
}

void Engine::cmdSetGlobals(const char *kglobals)
{
    appendCmd(POST, SET_KGLOBALS, kglobals);
}

void Engine::cmdSetHostname(const char *hostname)
{
    appendCmd(POST, SET_HOSTNAME, hostname);
}

void Engine::cmdSetMachine(const char* mach_type)
{
    appendCmd(POST, SET_MACHINE, mach_type);
}

void Engine::appendCmd(int type, int cmdid, const char *arg0, const char *arg1, const char *arg2, const char *arg3, const char *arg4)
{
    Cmd cmd;
    vector<string> arglist;
    
    if(arg0 != NULL) {
        arglist.push_back(arg0);
        if(arg1 != NULL) {
            arglist.push_back(arg1);
            if(arg2 != NULL) {
                arglist.push_back(arg2);
                if(arg3 != NULL) {
                    arglist.push_back(arg3);
                    if(arg4 != NULL) {
                        arglist.push_back(arg4);
                    }
                }
            }
        }
    }
    
    cmd.id = cmdid;
    cmd.args = arglist;

    cerr << "appendCmd: " << s_tags[cmdid] << endl;
    if(type == INSTALL){
        s_installcmds.push_back(cmd);
    }else{
        s_postcmds.push_back(cmd);
    }
}

bool Engine::do_mklabel(const string &devpath, const string &labeltype)
{
    PartedDevices all_dev;
    Device* dev = all_dev.device( devpath.c_str() );
    if ( !dev )
        return false;

    PartitionTable* part_table = dev->parttable();
    bool ret = part_table->create(labeltype.c_str());
    if ( ret )
        ret = part_table->commit();
    
    return ret;
}

bool Engine::do_mkpart_whole(const string &devpath, const string &index, const string &parttype, const string &fstype)
{
    PartedDevices all_dev;
    Device* dev = all_dev.device( devpath.c_str() );
    if ( !dev )
        return false;
 
    PartitionTable* part_table = dev->parttable();
    if (!part_table)
        return false;
    PartitionList* part_list = part_table->partlist();
    int i = atoi(index.c_str());
    int num = part_list->add_by_whole( i, parttype.c_str(), fstype.c_str() );
    bool ret = false;
    if ( num )
        ret = part_table->commit();

    if ( parttype == "extended" )
        return ret;
	
	//XX : dirty code, fixme in the future.
    char num_str[3];
    sprintf(num_str, "%d", num);
    string path;
    path = devpath + num_str;
	cerr<<"I'll wait for " << path << " is created."<<endl;
	for (i=0; i<20;i++)
	{
		sleep(1);
		if (access(path.c_str(), R_OK|W_OK) == 0)
		{
		    cerr<<path << " has been created successfully."<<endl;
			break;
		}
	}	
	
	if (i == 60)
	{
		cerr<<path << " isn't created successfully."<<endl;
		_errstr = path + "isn't created successfully.";
		return false;	
	}

    ret = do_mkfs( path, fstype );

    return ret;
}

bool Engine::do_mkpart_length(const string &devpath, const string &index, const string &parttype,
                              const string &fstype, const string &length)
{
    PartedDevices all_dev;
    Device* dev = all_dev.device( devpath.c_str() );
    if ( !dev )
        return false;
 
    PartitionTable* part_table = dev->parttable();
    if (!part_table)
        return false;
    PartitionList* part_list = part_table->partlist();
    int i = atoi(index.c_str());

    int num = part_list->add_by_length( i, parttype.c_str(), fstype.c_str(), length.c_str() );
    bool ret = false;
    if ( num )
        ret = part_table->commit();

    if ( parttype == "extended" )
        return ret;
	
    //XX : dirty code, fixme in the future.
    char num_str[3];
    sprintf(num_str, "%d", num);
    string path;
    path = devpath + num_str;
	cerr<<"I'll wait for " << path << " is created."<<endl;
	for (i=0; i<20;i++)
	{
		sleep(1);
		if (access(path.c_str(), R_OK|W_OK) == 0)
		{
		    cerr<<path << " has been created successfully."<<endl;
			break;
		}
	}	
	
	if (i == 60)
	{
		cerr<<path << " isn't created successfully."<<endl;
		_errstr = path + "isn't created successfully.";
		return false;	
	}

    ret = do_mkfs( path, fstype );

    return ret;
}
    
bool Engine::do_rmpart(const string &partpath)
{
    char* devpath;
    devpath = (char*) malloc( sizeof(char)* strlen(partpath.c_str()) );
    const char* p = partpath.c_str();
    int stop =0;

    while( *p != '\0' && !stop ) {
        if ( isdigit(*p) && *p != '0' )
            stop = 1;
        else
            p++;
    }

    assert( stop == 1 );
    int num = atoi(p);
    strncpy( devpath, partpath.c_str(), (p - partpath.c_str()) );
    *( devpath + ( p - partpath.c_str() ) ) = '\0';
    
    PartedDevices all_dev;
    Device* dev = all_dev.device( devpath );
    if ( !dev )
        return false;
 
    PartitionTable* part_table = dev->parttable();
    if (!part_table)
        return false;
    PartitionList* part_list = part_table->partlist();
    
    int ret = part_list->delete_num( num );
    if ( ret )
        ret = part_table->commit();
    return  ret;
}

bool Engine::do_rmallpart(const string &devpath)
{
    //XXX not implemented
}

bool Engine::do_mkfs(const string &partpath, const string &fstype)
{
    string cmd = "";

    // umount partpath
    cmd = "/bin/umount " + partpath;
    cerr<<"do_mkfs umount command line is:"<<cmd<<endl;
    system(cmd.c_str()); 
	
    cmd = "";	

    if (fstype == "swap" || fstype == "linux-swap")	{
	    cmd = "mkswap ";
	} else {
	    cmd = "mkfs -t ";
	    string type= "";
	    if (fstype == "fat32") {
            type = "vfat -F 32";
        } else if (fstype == "fat16") {
            type = "vfat -F 16";
        } else if ( fstype == "xfs" ) {
            type = "xfs -f ";
        } else if ( fstype == "reiserfs" ) {
            type = "reiserfs -f ";
        } else {
            type = fstype;
        }
	
	    cmd += type;
	    cmd += " ";
	}
	
    cmd += partpath;

    cerr<<"mkfs command line is:"<<cmd<<endl;

    if (system(cmd.c_str()) != 0) {
        cerr<<"mkfs error"<<endl;
        return false;
    }
	
    cmd = "";
    string num = "";
    string dev = "";
    int size = partpath.size();

    if ((partpath[size-2] >='1') && (partpath[size-2] <= '9')) {
        num = partpath.substr(size-2, 2);
        dev = partpath.substr(0, size-2);
    }
    else if ((partpath[size-1] >='1') && (partpath[size-1] <= '9')) {
        num = partpath.substr(size-1);
        dev = partpath.substr(0, size-1);
    }

    /*FIXME: sfdisk does not suppor GPT
    cmd = "sfdisk --id ";
    cmd += dev;
    cmd += " ";
    cmd += num;
    cmd += " ";


    if (fstype == "swap" || fstype == "linux-swap") {
        cmd += "82";
    } else if (fstype == "ext2" || fstype == "ext3" || fstype == "ext4") {
        cmd += "83";
    } else if (fstype == "fat32") {
        cmd += "c";
    } else if (fstype == "fat16") {
        cmd += "e";
    }
		
    cerr<<"sfdisk command line is:"<<cmd<<endl;
		
    if (system(cmd.c_str()) != 0) {
        cerr<<"sfdisk error"<<endl;
        return false;
    }
  */
    return true;
}


bool Engine::do_set_mountpoint(const string &devpath, const string &mountpoint, const string &fstype)
{
    fstab_struct tmp;

    tmp.devpath = devpath;
    tmp.mountpoint = mountpoint;

    if (fstype == "linux-swap")
        {
            tmp.mountpoint = "swap";
            tmp.fstype = "swap";
        }
    else if (fstype == "fat32" || fstype == "fat16")
		{
			tmp.fstype = "vfat";
		}
		else
        	{
				tmp.fstype = fstype;
			}
		
    if (mountpoint == "/"){
        _rootdir = "/tmp/rootdir/";
        _rootdev = devpath;

        tmp.mount_para 	= mount_para_def;
        tmp.backup 		= backup;
        tmp.self_test 	= self_test_root;
        _fstablist.push_front(tmp);

    } else if (mountpoint == "/boot/efi") {
        tmp.mount_para = "umask=0077,shortname=winnt";
        tmp.backup 		= no_backup;
        tmp.self_test 	= no_self_test;
        _fstablist.push_back(tmp);

    } else {
        tmp.mount_para 	= mount_para_def;
        tmp.backup 		= no_backup;
        tmp.self_test 	= no_self_test;
        _fstablist.push_back(tmp);
    }

    return true;
}

// groups is in form 'basic,kde,devel' etc
bool Engine::do_choosegroups(const string &groups)
{
    _rpm_groups.clear();
    string::size_type idx = groups.find(',');
    string::size_type start = 0;
    while (idx != string::npos) {
        string s = groups.substr(start, idx-start);
        if (!s.empty()) 
            _rpm_groups.push_back(s);
        debuglog("add group: %s\n", s.c_str());
        start = idx+1;
        idx = groups.find(',', start);
    }
    
    string s = groups.substr(start);
    if (!s.empty()) 
        _rpm_groups.push_back(s);

    return true;
}

bool Engine::do_add_package(const string package)
{
    _package_list.push_back(package);
    return true;
}

bool Engine::do_add_group(const string group)
{
    _package_list.push_back("@" + group);
    return true;
}

bool Engine::do_boot_install(const string &devpath)
{
    // postpone grub install at postscript stage to handle EFI
    _grub_install_device = devpath;
    return true;
}

bool Engine::do_set_kernel_param(const string &param)
{
    if ( set_grub_args( param.c_str() ) != 0 ) {
        debuglog("set kernel param error\n");
        return false;
    }
    return true;
}

bool Engine::do_set_lang(const string &locale)
{
    _locale = locale;
    string cmd = "echo \"LANG=" + _locale + "\" > /etc/locale.conf";
    _postscript.push_back(cmd);
    return true;
}

bool Engine::do_add_user(const string &username, const string &passwd)
{
    static bool setPreUser_flag = false;

    _postscript.push_back(string("/usr/bin/useradd -g users -G wheel,sys,video,audio,disk -m ") + username);

    if (passwd.size() == 0) {
        _postscript.push_back(string("passwd -d ") + username);
    } else {
        char cmd[512];
        sprintf(cmd, "echo -e '%s\n%s' | passwd %s", 
                passwd.c_str(), passwd.c_str(), username.c_str());
        _postscript.push_back(cmd);
    }
    _new_user_names.push_back(username);

    if (setPreUser_flag == false) {    
	    setPreUser_flag = true;
	    string cmd = "mkdir -p ";
	    string mk_kdmsts_dir = _rootdir + "var/lib/kdm/";
	    cmd += mk_kdmsts_dir;

	    if ( system(cmd.c_str()) != 0 ) {
		    _errstr = "mkdir /var/lib/kdm error";
		    return false;
		}
		
	    string kdmsts = _rootdir + "var/lib/kdm/kdmsts";
	    ofstream out(kdmsts.c_str(), ios::trunc);

	    if (out == NULL) {
		    _errstr = "create /var/lib/kdm/kdmsts error, pls check it!";
		    return false;
		}

	    out << "[PrevUser]" << endl;
	    out << ":0=" << username << endl;
	    out.close();
	}			

    return true;
}

bool Engine::do_set_root_passwd(const string &passwd)
{
    char cmd[1024];

    //XXX FIXME: bad code
    sprintf(cmd, "echo -e '%s\n%s' | passwd root", passwd.c_str(), passwd.c_str());
    _postscript.push_back(cmd);
    return true;
}

bool Engine::do_set_kl(const string &kxkb)
{
    string filename_kl = _rootdir + "usr/share/kde-settings/kde-profile/default/share/config/kxkbrc"; 
    ofstream file_kl( filename_kl.c_str(), ios::trunc);
    file_kl << kxkb;
	file_kl.flush();
	file_kl.close();
    string cmd = "sed -i -e \"s/;/\\n/g\" ";
    cmd += filename_kl;
    cerr << "cmd: " << cmd << endl;
    int ret = system(cmd.c_str());
    if ( ret != 0 ) {
        debuglog("do_set_kl failed.\n");
        return false;
    } else 
        return true;
}

bool Engine::do_set_tz(const string &kzoneinfo)
{
    string filename_tz = _rootdir + "usr/share/kde-settings/kde-profile/default/share/config/ktimezonedrc";
    ofstream file_tz( filename_tz.c_str(), ios::trunc);
    file_tz << kzoneinfo;
	file_tz.flush();
	file_tz.close();
    string cmd = "sed -i -e \"s/;/\\n/g\" ";
    cmd += filename_tz;
    cerr << "cmd: " << cmd << endl;
    int ret = system(cmd.c_str());
    if ( ret != 0 ) {
        debuglog("do_set_tz failed.\n");
        return false;
    } else 
        return true;
}

bool Engine::do_set_xorg_conf(const string &xorgconf)
{
    string filename_xorg = _rootdir + "etc/X11/xorg.conf";
    ofstream file_xorg( filename_xorg.c_str(), ios::trunc);
    file_xorg << xorgconf;
	file_xorg.flush();
	file_xorg.close();
    string cmd = "sed -i -e \"s/;/\\n/g\" ";
    cmd += filename_xorg;
    cerr << "cmd: " << cmd << endl;
    int ret = system(cmd.c_str());
    if ( ret != 0 ) {
        debuglog("do_set_xorg_conf failed.\n");
        return false;
    } else 
        return true;
}

bool Engine::do_set_kglobals(const string& kglobals)
{
    string filename_kg = _rootdir + "usr/share/kde-settings/kde-profile/default/share/config/kdeglobals"; 
    ofstream file_kg( filename_kg.c_str(), ios::trunc);
    file_kg << kglobals;
	file_kg.flush();
	file_kg.close();
    string cmd = "sed -i -e \"s/;/\\n/g\" ";
    cmd += filename_kg;
    cerr << "cmd: " << cmd << endl;
    int ret = system(cmd.c_str());
    if ( ret != 0 ) {
        debuglog("do_set_kglobals failed.\n");
        return false;
    } else 
        return true;
}

bool Engine::do_set_hostname(const string& hostname)
{
    // edit the hostname of the /etc/sysconfig/network.
    string filename = _rootdir + "etc/sysconfig/network";
    string hostnameline = "HOSTNAME=" + hostname;
    string cmd = "sed -i -e \"/HOSTNAME/s/.*/" + hostnameline + "/\" " + filename;
    cerr << "cmd: " << cmd << endl;
    int ret = system(cmd.c_str());
    if ( ret != 0 ) {
        debuglog("do_set_hostname failed.\n");
        return false;
    } else
        return true;
}

// copy image from livecd to disk		
bool Engine::copy_files(void (*progress)(int), double range)
{
    FILE *fp = NULL;
    string ext3fs_file = "/dev/mapper/live-osimg-min"; 
    //string root_partition_fstype = "";
    int ext3fs_file_fd;
    int root_dev_fd;
    
    // open the debug log file.
    ofstream out(debug_file, ios::app);
    if (out == NULL){
        _errstr = string("open ") + debug_file + " error, pls check it!";
        return false;
    }

    // check the path of root
    if (_rootdev == ""){
        _errstr = "root device is wrong, pls check it!";
        return false;
    }

    // install the base system. ----------------------------------------------------------    start   ---------------
    out<<"install base system start"<<endl;
    
    // umount root partition before copy.
    string cmd = "";
    cmd = "/bin/umount " + _rootdev;

    if (system(cmd.c_str()) != 0) {
        _errstr = "umount root device error";
    }

    // copy the image file.
    if ((ext3fs_file_fd = open(ext3fs_file.c_str(), O_RDONLY)) == -1) {
        _errstr = "open ext3fs.img error";
        return false;
    }
   
    cmd = "/sbin/dumpe2fs -h " + ext3fs_file;

    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        _errstr = "dumpe2fs ext3fs_file error";
        return false;	
    }
    
    const string pattern[] = {"Block count", "Block size"};
    unsigned long long size[] = {0,0};
    string str;
    int pos = 0;

    char line_buf[BUF_LENGTH_LINE] = {'\0'};
    while(!feof(fp)){
        fgets(line_buf, BUF_LENGTH_LINE, fp);
        str = line_buf;
        pos = str.find(":");

        if(pos == string::npos){
            continue;
        }

        for(int i=0; i < 2; i++){ //XXX sizeof
            if(str.substr(0, pos) == pattern[i]){
                // find it
                size[i] = atoi(str.substr(pos+1).c_str());
                cout<<"size is "<<size[i]<<endl;
            }
        }
    }	
    pclose(fp);

    root_dev_fd = open(_rootdev.c_str(), O_WRONLY);
    if (root_dev_fd == -1) {
        _errstr = "open root device error";
        return false;
    }
    
    double times = size[0]*size[1]/BUF_SIZE + 1; // copy one more block for remain data
    double fraction = (1 / (double)times) * (range - 2);   // last 2 range for e2fs cmds
    char *buf = (char *)malloc(BUF_SIZE);

    long long num_read = 0;
    double percent = 0;
    for(int i=0; i<times; i++)
        {
            if((num_read = read(ext3fs_file_fd, buf, BUF_SIZE)) > 0){
                if (write(root_dev_fd, buf, num_read) == -1){
                    _errstr = "write error";
                    return false;	
                }
		
                percent += fraction;
            
                if(progress) {
                    progress(percent);
                }
            }

            if(num_read == -1){
                _errstr = "read error";
                return false;
            }
        }
    free(buf);	
    close(root_dev_fd);
    close(ext3fs_file_fd);

    char cmdbuf[1024];
    // check filesystem first
    cmd = "e2fsck -f -y " + _rootdev;
	
    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        _errstr = " e2fsck root device error";
        return false;	
    }
    
    while(fgets(cmdbuf, 1024, fp)){
        out << cmdbuf << endl;
    }
    pclose(fp);
    
    if(progress) {
        progress(range-1);
    }

    // resize2fs
    cmd = "resize2fs -f " + _rootdev;

    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        _errstr = "resize2fs root device error";
        return false;	
    }
    
    while(fgets(cmdbuf, 1024, fp)){
        out << cmdbuf << endl;
    }
    pclose(fp);
	
    if(progress) {
        progress(range);
    }

    out<<"install base system end"<<endl;
    // install the base system. ----------------------------------------------------------   end   -----------------
	
    // move the subdirectory into independent partitions, like /usr /home. ---------------------   start  ----------
    out<<"adjust other partition start"<<endl;
    return prepareFileSystem();	
}

bool Engine::prepareFileSystem()
{
    bool yum_install_mode = _rpm_groups.size() == 0; 
    doMountRoot();

    ofstream out(debug_file, ios::app);
    if (out == NULL){
        _errstr = string("open ") + debug_file + " error, pls check it!";
        return false;
    }

    FILE *fp = NULL;
    string cmd;

    if (yum_install_mode) {
        char temp[] = "/tmp/engine.XXXXXX";
        char *tmp_mountpoint = mkdtemp(temp);

        if (tmp_mountpoint == NULL)	{
            _errstr = "mkdir /tmp/engine.XXXXXX error";
            return false;	
        }

        for(list<fstab_struct> ::iterator fstab_it = _fstablist.begin(); fstab_it != _fstablist.end(); ++fstab_it) {
            if (fstab_it->mountpoint == "/boot")
                _boot_partition = fstab_it->devpath;

            if ( fstab_it->mountpoint == "/" 
                    || fstab_it->mountpoint == "/mnt" 
                    || fstab_it->mountpoint == "swap" 
                    || fstab_it->mountpoint == "/sys"
                    || fstab_it->mountpoint == "/proc" 
                    || fstab_it->mountpoint == "/dev/pts"
                    || fstab_it->mountpoint == "/dev/shm" ) {
                continue;
            }

            cmd = "/bin/mount -t " + fstab_it->fstype + " " + fstab_it->devpath + " " + tmp_mountpoint;
            if (system(cmd.c_str()) != 0) {
                _errstr = "mount " + fstab_it->devpath + " error";
                return false;	
            }

            //        cmd = "mv -f " + _rootdir + fstab_it->mountpoint.substr(1) + "/* " + tmp_mountpoint; 	
            cmd = "cp -a " + _rootdir + fstab_it->mountpoint.substr(1) + "/* " + tmp_mountpoint; 	
            system(cmd.c_str());

            cmd = "/bin/umount ";
            cmd += tmp_mountpoint;
            if (system(cmd.c_str()) != 0) {
                _errstr = "umount tmp_mountpoint error";
                return false;	
            }
        }

        errno = 0;
        if (rmdir(tmp_mountpoint) != 0) {
            perror(strerror(errno));
            cerr << "rmdir " << tmp_mountpoint <<  "error" << endl;
        }

        out<<"adjust other partition end"<<endl; 
        // move the subdirectory into independent partitions, like /usr /home. ----------------------   end   ----------

    }

    doSetupFstab();
    return true;
}

bool Engine::doMountRoot()
{

    _boot_partition = _rootdev;
    mkdir(_rootdir.c_str(), 0755);

    string cmd = "/bin/mount " + _rootdev + " " + _rootdir; 
    if (system(cmd.c_str()) != 0) {
        _errstr = "mount root device error";
        return false;	
    }
    return true;
}

bool Engine::doSetupFstab()
{
    
    ofstream out(debug_file, ios::app);
    if (out == NULL){
        _errstr = string("open ") + debug_file + " error, pls check it!";
        return false;
    }

    FILE *fp = NULL;
    string cmd;


    out<<"create /etc/fstab using uuid start"<<endl;
    // create fstab	
    string fstab_str = _rootdir + "etc/fstab";
    mkdir((_rootdir + "etc").c_str(), 0755);
    ofstream out_fstab(fstab_str.c_str(), ios::trunc);
	
    if(!out_fstab) {
        _errstr = "create /etc/fstab error";
        return false;
    }
    
    out_fstab.setf(ios::left, ios::adjustfield);

    string uuid = "";
    char uu[BUF_LENGTH_LINE] = {'\0'};

    for(list<fstab_struct> ::iterator fstab_it = _fstablist.begin(); fstab_it != _fstablist.end(); ++fstab_it) 
	{
        if ( fstab_it->mountpoint == "/sys" 
             || fstab_it->mountpoint == "/proc" 
             || fstab_it->mountpoint == "/dev/pts" 
             || fstab_it->mountpoint == "/dev/shm"
             || fstab_it->mountpoint == "swap") {
            continue;
        }
 
		cmd = "/sbin/blkid -s UUID -o value " + fstab_it->devpath; 
		if ((fp = popen(cmd.c_str(), "r")) == NULL) {
				_errstr = "uuidgen " + fstab_it->devpath + " error";
				return false;	
		}

		memset(uu, '\0', BUF_LENGTH_LINE);	
		fscanf(fp, "%s", uu);
		pclose(fp);
		uuid = uu;
		string dev = "UUID=" + uuid;

		out_fstab << setw(50) << dev; 
		out_fstab << setw(20) << fstab_it->mountpoint;
		out_fstab << setw(20) << fstab_it->fstype;
		out_fstab << setw(30) << fstab_it->mount_para;
		out_fstab << setw(20) << fstab_it->backup;
		out_fstab << setw(20) << fstab_it->self_test<<endl;
    } 

    // mount all partitions
    mkdir((_rootdir + "dev/shm").c_str(), 0755); // not existed, make it
    
    for(list<fstab_struct> ::iterator fstab_it = _fstablist.begin(); fstab_it != _fstablist.end(); ++fstab_it) {
        if (fstab_it->mountpoint == "/" || fstab_it->mountpoint == "swap")
            continue;
        
        string mnt_path = _rootdir + fstab_it->mountpoint.substr(1);
        cmd = "mkdir -p " + mnt_path + " && /bin/mount -t " + fstab_it->fstype 
            + " " + fstab_it->devpath + " " + mnt_path;
		
        if ((fp = popen(cmd.c_str(), "r")) == NULL) {
            _errstr = "/bin/mount " + fstab_it->devpath + " error";
            return false;	
        }
		
        pclose(fp);
    }		

    out_fstab.close();
    out<<"create /etc/fstab using uuid end"<<endl;
    return true;
}
