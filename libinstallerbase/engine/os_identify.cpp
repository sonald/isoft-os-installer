#include "parted++.h"

#include <iostream>
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
		if (!table->read())
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
int OSIdentify::judge_linux(const char *device)
{
	char os[OS_TYPE_LENGTH];

	if (device == NULL)
	{
		fprintf(stderr, "device is NULL\n");
		return -1;
	}
		
	if (get_os_type_str(device, os) == 0)
		return 0;
	else
		return -1;
}

// judge if it's windows
// if it's windows return 0 else -1
int OSIdentify::judge_windows(const char *device)
{
	char os[OS_TYPE_LENGTH];

	if (device == NULL)
	{
		fprintf(stderr, "device is NULL\n");
		return -1;
	}
		
	if (get_os_type_str(device, os) == 1)
		return 0;
	else
		return -1;
}

// get an os type string
// if not find an os return NULL
// don't deal with boot partition
// if find linux, return 0;
// if find windows, return 1;
// if find nothing, return -1;
int OSIdentify::get_os_type_str(const char *device, char *os_type)
{
	int ret = -1;
	string mountPath;
	string cmd;
	char *found = NULL;
	const char* prefix_alternats[] = {
		"/winnt/system32/config",
		"/windows/system32/config"
	};

	if (device == NULL || os_type == NULL)
	{
		fprintf(stderr, "device or os_type is NULL\n");
		return -1;
	}
	
	memset(os_type, '\0', OS_TYPE_LENGTH);

	if (mount_fs(device, mountPath) == false)
	{
		std::cerr << "mount error\n";
		return -1;
	}

	// linux first
	if (find_linux_by_issue(os_type, mountPath) == 0)
	{
		ret = 0;
	}
	else
	{	// windows second
		//k 2 - 2 types of windows/system
		for(int i = 0; i < 2; i++ ) 
		{
			string tmp = mountPath + prefix_alternats[i];
			if ( _stat_insensitive( tmp.c_str(), &found ) >= 0 ) 
			{
				if (find_windows_by_register(os_type, mountPath.c_str()) == 0)
				{
					ret = 1;
				}
			}
		}
	}

	// not found
	cmd = "umount " + mountPath;
	if (system(cmd.c_str()) != 0)
	{
		fprintf(stderr, "%s error\n", cmd.c_str());
	}

	if (rmdir(mountPath.c_str()) != 0)
	{
		fprintf(stderr, "rmdir %s error\n", mountPath.c_str());
	}
	
	return ret;
}

// get an os type struct
// if find an grub entry, return 0 else -1
int OSIdentify::get_os_info(const char *device, struct os_info *os_info_p)
{
	int ret = 0;
	int value = -1;
	string mountPath;

	if (device == NULL || os_info_p == NULL )
	{
		std::cerr << "device or os_info_p is NULL\n";
		return -1;
	}

	// iniatialize struct os_info 	
	os_info_p->is_linux = 0;
	os_info_p->is_windows = 0;
	os_info_p->is_boot = 0;
	os_info_p->os_num = 0;
	memset(os_info_p->os_type, '\0', OS_TYPE_LENGTH);
	memset(os_info_p->device_name, '\0', OS_TYPE_LENGTH);
	strncpy(os_info_p->device_name, device, DEVICE_NAME_LENGTH);
	os_info_p->kernel_info_p = NULL;

	if (mount_fs(device, mountPath) == false)
	{
		std::cerr << "mount error\n";
		return -1;
	}
	
	ret = get_os_type_str(device, os_info_p->os_type);
	
	switch (ret) 
	{
		case 1: 
				value = find_windows_grub_entry(os_info_p, mountPath);
				break;
		default:
				value = find_linux_grub_entry(os_info_p, mountPath);
				break;
	}
				
	string cmd = "umount " + mountPath;
	system(cmd.c_str());
	rmdir(mountPath.c_str());

	return value;
}

// free a os_info struct, to save memory 
void OSIdentify::free_os_info(struct os_info *os_info_p)
{
	struct kernel_info *p = NULL;
	struct kernel_info *p_del = NULL;
	
	if (os_info_p == NULL )
		return;
	
	if (os_info_p->kernel_info_p == NULL )
		return;
	
	p = os_info_p->kernel_info_p;

	while(p != NULL)
	{
		p_del = p;
		p = p->next;
		free(p_del);		
	}	

	return;
}

// try to mount a device 
//k success return dir in parameter mountParth, FAIL return NULL
bool OSIdentify::mount_fs(const char *device, string& mountPath)
{
	string cmd; 

	char temp[] = "/tmp/osidentify.XXXXXX"; 
	char *dir = mkdtemp(temp);
	if (dir == NULL) {
		fprintf(stderr, "Make temporary directory failed.\n");
		return false;
	}
	string fs_type = m_partsType[device];
	string mountType;
	if ("fat16" == fs_type || "fat32" == fs_type)
		mountType = "vfat";
	else if (fs_type == "linux-swap")
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
	int status = system(cmd.c_str()); 

	if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) 
	{
		std::cerr << "mount partition " << device << " failed.";
		rmdir(dir);
		return false;
	}

	mountPath = dir;
	return true;
}

// find linux by issue file, just for rfinstaller diaplay.
int OSIdentify::find_linux_by_issue(char *os_type, const string &mountPath)
{
	FILE *fp;
	char *p = NULL;
	char buf[BUF_LENGTH_LINE] = {'\0'};

	string mount = mountPath + "/etc/issue";
	if ((fp = fopen(mount.c_str(), "r")) != NULL)
	{
		// skip blank line
		while (!feof(fp))
		{	
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
	}
	else
	{
		fprintf(stderr, "fopen issue error\n");	
		return -1;
	}
}

// find windows by registry, just for rfinstaller to display.
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

// find linux initrd file
int OSIdentify::find_linux_initrd_file_name(const char *str, string mountPath, bool has_boot_partition)
{
	DIR *pdir = NULL;
	struct dirent *pent = NULL;
	
	if (has_boot_partition == 1)
	{	
		if ((pdir = opendir((mountPath + "/boot").c_str())) == NULL)
		{
			return -1;
		}
	}
	else if (has_boot_partition == 0)
		{	
		 	if ((pdir = opendir((mountPath).c_str())) == NULL)
			{
				return -1;
			}
		}
		else
			return -1;
	
	string tmp = string("initrd-") + str + ".img";

	while (1)
	{
		if ((pent = readdir(pdir)) == NULL)
			break;
	
		if (strcmp(pent->d_name, tmp.c_str()) == 0)
		{
			closedir(pdir);
			return 0;
		}
	}

	closedir(pdir);
	return -1;
}

// find linux kernel file
int OSIdentify::find_linux_vmlinuz_file_name(struct os_info *os_info_p, string mountPath, bool has_boot_partition)
{
	char *p = NULL;
	char *tmp = NULL;
	DIR *pdir = NULL;
	struct dirent *pent = NULL;

	struct kernel_info **kernel_p = &os_info_p->kernel_info_p;

	if (has_boot_partition == true)
	{ 
		if ((pdir = opendir((mountPath + "/boot").c_str())) == NULL)
		{
			return -1;
		}
	}
	else if ((pdir = opendir((mountPath).c_str())) == NULL)
		{
			return -1;
		}

	while (1)
	{
		if ((pent = readdir(pdir)) == NULL)
			break;

		if ((p = strstr(pent->d_name, "vmlinuz-")) != NULL)	
		{
			p += strlen("vmlinuz-");  		
			if (find_linux_initrd_file_name(p, mountPath, has_boot_partition) == 0)
			{
				// find a grub entry
				(*kernel_p) = (struct kernel_info *) malloc(sizeof(struct kernel_info));
				
				if ((*kernel_p) == NULL)
				{
					fprintf(stderr, "malloc kernel_p error\n");
					return -1;
				}

				(*kernel_p)->next = NULL;
				memset((*kernel_p)->vmlinuz_file_name, 0, BUF_LENGTH_LINE);
				memset((*kernel_p)->initrd_file_name , 0, BUF_LENGTH_LINE);
		
				strcpy((*kernel_p)->kernel_release, p);

				tmp = (*kernel_p)->initrd_file_name;
				strcpy(tmp, "initrd-");
				tmp += strlen("initrd-");
				strcpy(tmp, p);
				tmp += strlen(p);
				strcpy(tmp, ".img");

				tmp = (*kernel_p)->vmlinuz_file_name;
				strncpy(tmp, pent->d_name, BUF_LENGTH_LINE);
				
				os_info_p->is_linux = 1;
				os_info_p->is_windows = 0;
				os_info_p->is_boot = (has_boot_partition == true) ? 0 : 1;
				os_info_p->os_num++;
				kernel_p = &((*kernel_p)->next);
				continue;
			}	
		}
	}

	closedir(pdir);

	return  (os_info_p->os_num > 0) ? 0 : -1;
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

int OSIdentify::find_linux_grub_entry(struct os_info *os_info_p, const string mountPath)
{
	bool has_boot_partition = false;
	DIR *pdir = NULL;

	if ((pdir = opendir((mountPath + "/boot").c_str())) != NULL)
	{
		has_boot_partition = true;
	}
	else if ((pdir = opendir((mountPath).c_str())) != NULL)
		{
			has_boot_partition = false;
		}
		else 
		{
			fprintf(stderr, "can't open mountPath %s\n", mountPath.c_str());			
			closedir(pdir);
			return -1;	
		}

	if (find_linux_vmlinuz_file_name(os_info_p, mountPath, has_boot_partition) == 0)
	{
		if (os_info_p->is_boot == 1)
		{
			memset(os_info_p->os_type, '\0', OS_TYPE_LENGTH);
			strcpy(os_info_p->os_type, "Linux ");
		}
			
		closedir(pdir);
		return 0;
	}
	else
	{
		closedir(pdir);
		return -1;
	}
}

int OSIdentify::find_windows_grub_entry(struct os_info *os_info_p, const string mountPath)
{
	char *found = NULL;
	const char* prefix_alternats[] = {
		"/winnt/system32/config",
		"/windows/system32/config"
	};
	
	//k 2 - 2 types of windows/system
	for(int i = 0; i < 2; i++ ) 
	{
		string tmp = mountPath + prefix_alternats[i];
		if ( _stat_insensitive( tmp.c_str(), &found ) >= 0 ) 
		{
			if (find_windows_by_register(os_info_p->os_type, mountPath.c_str()) == 0) 
			{
				os_info_p->is_linux = 0;
				os_info_p->is_windows = 1;
				os_info_p->is_boot = 0;
				os_info_p->os_num = 1;
				return 0;
			}
		}
	}

	return -1;
}
