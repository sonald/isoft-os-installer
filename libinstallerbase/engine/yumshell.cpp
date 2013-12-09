#include "yumshell.h"
#include <cstring>
#include <stdlib.h>

#define DEBUG_MODE        
#ifdef   DEBUG_MODE  
#define   debuglog     printf  
#else  
#define   debuglog
#endif

#define LINEMAX 1024

YumShell::YumShell(const list<string> &pkgs, const string &rootdir, const string &conffile)
{
    getRealPkgs(pkgs);
    if (rootdir == "/") {
        _rootdir = "";
    } else {
        _rootdir = rootdir;
    }
    _conffile = conffile;
}

YumShell::~YumShell()
{
}

void YumShell::setProgressRange(int min, int max)
{
    _progmin = min;
    _progmax = max;
}

int YumShell::getInstallNumber(void)
{
    return _pkgs.size();
}

bool YumShell::install(void (*progress)(int))
{
    FILE *fp;
    char line[LINEMAX];
    list<string>::const_iterator i;
    string install_command("yum");
    string install_root(" --installroot=");
    
    install_root += _rootdir;
    install_command = install_command + install_root + " -c /etc/yum_livecd.conf -y install";

    for(i=_pkgs.begin(); i!=_pkgs.end(); i++) {
        install_command += " \"" + *i +"\"";
    }
    /*
      int j;
      for (j=1, i=_pkgs.begin(); i!=_pkgs.end(); j++, i++) {
      progress(j);
      cout << *i << endl;
      }
    */

    //cout << "install_command: " << install_command << endl;

    if (_pkgs.empty()) {
        _errstr = "no package to install";
        return true;
    }
    
    if ((fp = popen(install_command.c_str(), "r")) == NULL) {
        _errstr = "install command failed";
        return false;
    }

    while (fgets(line, LINEMAX, fp)) {
        if (strncmp(line, "Running Transaction\n", LINEMAX) == 0) {
            break;
        }
    }

    while (fgets(line, LINEMAX, fp)) {
        if (strncmp(line, "\n", LINEMAX) == 0) {
            break;
        }

        int index = 0;
        double num = 0;

        //XXX rewrite with atoi
        while (line[index]!='[' && line[index]!='\n') {
            index++;
        }

        if (line[index] != '[') {
            continue;
        }

        while ((line[index]<'0') || (line[index]>'9')) {
            index++;
        }
        /*
          while (line[index]!='/') {
          num = num * 10 + (line[index] - '0');
          index++;
          }
        */
        char tmp_num[LINEMAX];
        int start = 0;
        while (line[index]!='/') {
            tmp_num[start++] = line[index++];
        }
        tmp_num[start] = '\0';
        num = atoi(tmp_num);
 
        num = _progmin + (_progmax - _progmin) * num / _pkgs.size();

        if(progress) {
            progress(num);
        }

        // not need to call sleep, use I/O block for spinning, just like event triger :)
        //sleep(1);
    }

    pclose(fp);

    return true;
}

bool YumShell::getRealPkgs(const list<string> &pkgs)
{
    list<string>::const_iterator i;
    list<string> groups;
    list<string> packages;

    string tmp_pkg;

    FILE *fp;
    char line[LINEMAX];

    string group_check_command("echo n | yum groupinstall");
    string package_check_command("echo n | yum install");

    string group;
    int size;
 
    for (i=pkgs.begin(); i!=pkgs.end(); i++) {
        if ((*i)[0] == '@') {
            group = "";
            size = (*i).size();
            for (int j=1; j<size; j++) {
                group += (*i)[j];
            }
            groups.push_back(group);
        } else {
            packages.push_back(*i);
        }
    }

    /*
      for (i=groups.begin(); i!=groups.end(); i++) {
      cout << "GROUP" << ":\t" << *i << endl;
      }
    
      for (i=packages.begin(); i!=packages.end(); i++) {
      cout << "PACKS" << ":\t" << *i << endl;
      }
    */

    for (i=groups.begin(); i!=groups.end(); i++) {
        group_check_command = group_check_command + " \"" + *i +"\"";
    }
    
    for (i=packages.begin(); i!=packages.end(); i++) {
        package_check_command = package_check_command + " \"" + *i +"\"";
    }

    /*
      cout << "group_check_command: " << group_check_command << endl;
      cout << "package_check_command: " << package_check_command << endl;
    */

    // get real packages from group
    if (!groups.empty()) {
        if ((fp = popen(group_check_command.c_str(), "r")) == NULL) {
            return false;
        }

        while (fgets(line, LINEMAX, fp) != NULL) {
            if (strncmp(line, "Installing:\n", LINEMAX) != 0) {
                continue;
            } else {
                break;
            }
        }

        while ((fgets(line, LINEMAX, fp) != NULL) && (strncmp(line, "\n", LINEMAX) != 0)) {
            if (line[0] != ' ') {
                continue;
            } else {
                tmp_pkg = "";
                for (int j=1; line[j]!=' '; j++) {
                    tmp_pkg += line[j];
                }
                _pkgs.push_back(tmp_pkg);
            }
        }

        pclose(fp);
    }
    
    /*
      for (i=_pkgs.begin(); i!=_pkgs.end(); i++) {
      cout << _pkgs.size() << "\t" << *i << endl;
      }
    */

    // get real packages from package
    if (!packages.empty()) {
        if ((fp = popen(package_check_command.c_str(), "r")) == NULL) {
            return false;
        }

        while (fgets(line, LINEMAX, fp) != NULL) {
            if (strncmp(line, "Installing:\n", LINEMAX) != 0) {
                continue;
            } else {
                break;
            }
        }

        while ((fgets(line, LINEMAX, fp) != NULL) && (strncmp(line, "\n", LINEMAX) != 0)) {
            if (line[0] != ' ') {
                continue;
            } else {
                tmp_pkg = "";
                for (int j=1; line[j]!=' '; j++) {
                    tmp_pkg += line[j];
                }
                list<string>::const_iterator tmp;
                for (tmp=_pkgs.begin(); tmp!=_pkgs.end(); tmp++) {
                    if (tmp_pkg == *tmp) {
                        break;
                    }
                }
                if (tmp == _pkgs.end()) {
                    _pkgs.push_back(tmp_pkg);
                }
            }
        }

        pclose(fp);
    }
    
    /*
      for (i=_pkgs.begin(); i!=_pkgs.end(); i++) {
      cout << _pkgs.size() << "\t" << *i << endl;
      }
    */
    return true;
}

bool YumShell::test(void)
{
    list<string> pkgs;

    pkgs.push_back("@KDE (K Desktop Environment)");
    pkgs.push_back("@KDE Software Development");

    string rootdir = "/media";

    YumShell *y = new YumShell(pkgs, rootdir);

    cout << y->getInstallNumber() << endl;

    y->setProgressRange(0, 100);

    y->install(NULL);
}
