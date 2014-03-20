#include "parted++.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mount.h>
#include <unistd.h>
#include <glib.h>

#include "os_identify.h"

//k just fill m_partsType
OSIdentify::OSIdentify()
{
	PartedDevices *devices = new PartedDevices;
	for (int i = 0; i < devices->count(); ++i) {
		Device *disk = devices->device(i);
		if (!disk)
			continue;

		PartitionTable *table = disk->parttable();
        if (!table)
			continue;

		PartitionList *partList = table->partlist();
		for (int j = 0; j < partList->count(); ++j) {
			Partition *part = partList->part_index(j);
			string name = part->path();
			if (name[name.size()-2] != '-')
				m_partsType[name] = part->fs_type_name();
				//k type ref libparted/labels/rdb.c:line 898
		}
	}

	delete devices;
	devices = 0;
} 

// judge if it's linux
// if it's linux return 0 else -1
bool OSIdentify::judge_linux(const char *device)
{
	char os[OS_TYPE_LENGTH];

	if (device == NULL) {
		fprintf(stderr, "device is NULL\n");
		return false;
	}
		
	return (get_os_type_str(device, os) == OS_LINUX);
}

// judge if it's windows
// if it's windows return 0 else -1
bool OSIdentify::judge_windows(const char *device)
{
	char os[OS_TYPE_LENGTH];

	if (device == NULL) {
		fprintf(stderr, "device is NULL\n");
		return false;
	}
		
	return (get_os_type_str(device, os) == OS_WINDOWS);
}

// get an os type string
// if not find an os return NULL
// don't deal with boot partition
// if find linux, return 0;
// if find windows, return 1;
// if find nothing, return -1;
OSIdentify::OSType OSIdentify::get_os_type_str(const char *device, char *os_type)
{
	OSType ret = OS_UNKNOWN;
	string mountPath;
	string cmd;
	char *found = NULL;
	const char* prefix_alternats[] = {
		"/winnt/system32/config",
		"/windows/system32/config"
	};

	if (device == NULL || os_type == NULL) {
		fprintf(stderr, "device or os_type is NULL\n");
		return ret;
	}
	
	memset(os_type, '\0', OS_TYPE_LENGTH);

	if (mount_fs(device, mountPath) == false) {
		std::cerr << "mount error\n";
		return ret;
	}

	// linux first
	if (find_linux_by_release(os_type, mountPath) == 0) {
        ret = OS_LINUX;
    } else if (find_linux_by_issue(os_type, mountPath) == 0) {
		ret = OS_LINUX;
	} else {	// windows second
		//k 2 - 2 types of windows/system
		for(int i = 0; i < 2; i++ ) {
			string tmp = mountPath + prefix_alternats[i];
			if ( _stat_insensitive( tmp.c_str(), &found ) >= 0 ) {
				if (find_windows_by_register(os_type, mountPath.c_str()) == 0) {
					ret = OS_WINDOWS;
				}
			}
		}
	}

	// not found
	cmd = "umount " + mountPath;
	if (system(cmd.c_str()) != 0) {
		fprintf(stderr, "%s error\n", cmd.c_str());
	}

	if (rmdir(mountPath.c_str()) != 0) {
		fprintf(stderr, "rmdir %s error\n", mountPath.c_str());
	}
	
	return ret;
}

// try to mount a device 
//k success return dir in parameter mountParth, FAIL return NULL
bool OSIdentify::mount_fs(const char *device, string& mountPath)
{
	string cmd; 

	char temp[] = "/tmp/osidentify.XXXXXX"; 
	char *dir = mkdtemp(temp);
	if (dir == NULL) {
        std::cerr << "Make temporary directory failed.\n";
		return false;
	}
	string fs_type = m_partsType[device];
	string mountType;
	if ("fat16" == fs_type || "fat32" == fs_type)
		mountType = "vfat";
	else if (fs_type.find("linux-swap") != fs_type.npos)
		return false;
	else if (fs_type == "sun-ufs" || fs_type == "hp_ufs")
		mountType = "ufs";
	else if (fs_type == "Unknown")
		return false;
	else
		mountType = fs_type;

	if (mountType.size() == 0)
		return false;

	cmd = "/bin/mount  -t " + mountType + " " + device + " " + dir;
    std::cerr << cmd << endl;
	int status = system(cmd.c_str()); 
	if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
		std::cerr << "mount partition " << device << " failed.";
		rmdir(dir);
		return false;
	}

	mountPath = dir;
	return true;
}

int OSIdentify::find_linux_by_release(char *os_type, const string &mountPath)
{
	string release_file = mountPath + "/etc/os-release";
    ifstream ifs;
    ifs.open(release_file, ::ios_base::in);

    for (string line; getline(ifs, line); ) {
        if (line.find("NAME=") == 0) {
            string release = line.substr(5, line.size()-1);
            auto end = ::remove_if(release.begin(), release.end(),
                    [](char c) { return c == '"' || c == '\''; });
            strncpy(os_type, release.substr(0, end - release.begin()).c_str(), 
                    OS_TYPE_LENGTH-1);
            return 0;
        }
    }

    return -1;
}

// find linux by issue file, just for cetcosinstaller display.
int OSIdentify::find_linux_by_issue(char *os_type, const string &mountPath)
{
	FILE *fp;
	char *p = NULL;
	char buf[BUF_LENGTH_LINE] = {'\0'};

	string mount = mountPath + "/etc/issue";
	if ((fp = fopen(mount.c_str(), "r")) != NULL) {
		// skip blank line
		while (!feof(fp)) {	
			fgets(buf, BUF_LENGTH_LINE, fp);
			if (strcmp(buf, "\n") != 0)
				break;
		}
		
		p = strchr(buf, '\n');
		if (p != NULL)
			*p = '\0';
		
		p = strchr(buf, '(');
		if (p != NULL)
			*p = '\0';
		
		strncpy(os_type, buf, OS_TYPE_LENGTH-1);

		fclose(fp);
		return 0;	
	} else {
		fprintf(stderr, "fopen issue error\n");	
		return -1;
	}
}

// find windows by registry, just for cetcosinstaller to display.
int OSIdentify::find_windows_by_register(char *os_type, string mountPath)
{
    const char *kern_path = "/windows/system32/kernel32.dll";
	char *found = NULL;

    string tmp = string(mountPath) + kern_path;
    if (_stat_insensitive( tmp.c_str(), &found ) >= 0) 
    {
        return 0;
    }
   	
	return -1;
}

int OSIdentify::_stat_insensitive(const char* in_path, char** out_path) 
{
	g_debug( "in_path: %s", in_path );
	int path_size = strlen(in_path);
	char **parts = g_strsplit_set( in_path, "/", path_size );
	char *parent = (char*) malloc( path_size + 1 );
	parent[0] = 0;

	const gchar *part;	
	int i = 1; // start from 1, because here parts[0] == "", see g_strsplit_set
	GDir *dir = NULL;
	GError *error = NULL;
	strcat( parent, "/" );
	while( parts[i] != NULL ) {
		dir = g_dir_open( parent, 0, &error );
		if ( error != NULL ) {
			g_debug( "error: %s", error->message );
			return -1;
		}
		// case-insensitive search part
		while( (part = g_dir_read_name(dir)) != NULL ) {
			if ( g_ascii_strcasecmp(part, parts[i]) == 0 )
		//	if ( memcmp(part, parts[i], strlen(part)) == 0 )
				goto label_part_match;
		}
		// nothing matched, then error
		if ( part )
			g_debug( "match part failed: %s", part );
		if ( out_path ) *out_path = NULL;
		free( parent );
		g_strfreev( parts );
		g_dir_close( dir );
		return -1;
		
	label_part_match:
		if ( !(parent[0] == '/' && parent[1] == 0) )
			strcat( parent, "/" );
		strcat( parent, part );
		
		g_dir_close( dir );
		i++;
	}//~ while( *parts[i]....

	if ( out_path ) {
		g_debug( "found real path %s", parent );
		*out_path = parent;
	}
	g_strfreev( parts );
	return 0;
}

