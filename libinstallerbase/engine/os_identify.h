/*
 * 	Copyright 1999, 2008 Neil Kang <kaikang@redflag-linux.com>
 *
 *   	This program is free software; you can redistribute it and/or modify
 *   	it under the terms of the GNU General Public License as
 *   	published by the Free Software Foundation; either version 2,
 *   	or (at your option) any later version.
 *
 *   	This program is distributed in the hope that it will be useful,
 *   	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   	GNU General Public License for more details
 *
 *   	You should have received a copy of the GNU Library General Public
 *   	License along with this program; if not, write to the
 *   	Free Software Foundation, Inc.,
 *   	51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef OS_IDENTIFY_H
#define OS_IDENTIFY_H

#include <string>
#include <map>
using namespace std;

#define FILE_SYSTEM_NUM 	10			// check at most 10 file system when mount
#define OS_TYPE_LENGTH  	50
#define DEVICE_NAME_LENGTH  16
#define BUF_LENGTH_LINE		128
#define BUF_SIZE			(8 * 1024 * 1024)

struct kernel_info
{
	char   kernel_release[BUF_LENGTH_LINE];	
	char   vmlinuz_file_name[BUF_LENGTH_LINE];	
	char   initrd_file_name[BUF_LENGTH_LINE];	
	struct kernel_info *next;
};

struct os_info
{
	char 	is_linux; 
	char 	is_windows;
	char 	is_boot;
	char    os_num;
	char 	device_name[DEVICE_NAME_LENGTH]; 
	char 	os_type[OS_TYPE_LENGTH];
	struct  kernel_info *kernel_info_p;
};

class OSIdentify
{
public:
	OSIdentify();

	// judge if it's linux
	// if it's linux return 0 else -1
	int judge_linux(const char *device);

	// judge if it's windows
	// if it's windows return 0 else -1
	int judge_windows(const char *device);

	// get an os type string, for cetcosinstaller use
	// if find, return 0, and store os type info into char *os_type;
	// else return -1
	int get_os_type_str(const char *device, char *os_type);

	// get an os type struct, for gen_grub_conf use
	// if find an os return 0 else -1
	int get_os_info(const char *device, struct os_info *os_info_p);

	// free os_info struct
	void free_os_info(struct os_info *os_info_p);

private:
	int 	find_linux_by_issue(char *os_type, const string &mountPath);
	int 	find_linux_by_release(char *os_type, const string &mountPath);
	int 	find_windows_by_register(char *os_type, const string mountPath);

	int 	find_linux_grub_entry(struct os_info *os_info_p, const string mountPath);
	int 	find_windows_grub_entry(struct os_info *os_info_p, const string mountPath);

	int 	find_linux_vmlinuz_file_name(struct os_info *os_info_p, const string mountPath, bool has_boot_partition);
	int 	find_linux_initrd_file_name(const char *str, const string mountPath, bool has_boot_partition);

	bool 	mount_fs(const char *device, string &mountPath);
	int 	_stat_insensitive(const char* in_path, char** out_path); 

	map<string, string> m_partsType;
};

#endif
