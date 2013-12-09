#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <parted/parted.h>
//#include "installengine.h"
#include "yumshell.h"
#include "os_identify.h"
#include "gen_grub_conf.h"

#define GRUB_CONF_FILE                  "/tmp/rootdir/boot/grub/grub.conf"
#define STRING_LENGTH                   80
#define SHORT_STRING_LENGTH             10
#define DEV_LENGTH                      8
#define PARTITION_LENGTH                64
#define GRUB_PARTITION_LENGTH           64
#define IDE_NUM                         8
#define SCSI_NUM                        16

static const char *dt7_default_boot_sc 		= "0";
static const char *dt7_timeout_sc 		= "30";
static const char *dt7_gfxmenu_sc 		= "/boot/message";
static const char *dt7_splashimage_sc 		= "/boot/grub/splash.xpm.gz";
static const char *dt7_gfxmenu_boot_sc 		= "/message";
static const char *dt7_splashimage_boot_sc 	= "/grub/splash.xpm.gz";
static const char *dt7_title_sc		        = "RedFlag Linux Desktop 7.0";
static char dt7_vmlinuz_name_sc[STRING_LENGTH]  = "\0";
static char dt7_kernel_release_sc[STRING_LENGTH]= "\0";
static char dt7_initrd_name_sc[STRING_LENGTH]   = "\0";
static char dt7_grub_args_sc[STRING_LENGTH]   = "\0";
static const char *dt7_kernel_ro_sc 		= "ro";
static const char *dt7_kernel_vga_sc 		= "788";
static const char *dt7_kernel_splash_sc 	= "quiet rhgb";
static const char *windows_makeactive_sc	= "makeactive";
static const char *windows_chainloader_sc	= "chainloader +1";
static const char *grub_install_sc		= "/sbin/grub-install --no-floppy --root-directory=/tmp/rootdir ";

static struct partition *partition_head_g 	= NULL;		// partition list head

struct partition
{
    char dev[DEV_LENGTH];                		// like hd or sd, end up with \0  
    char dev_num;		             		// like a\b\c\d
    int  partition_num;                                 // 1~n
    char partition_name[PARTITION_LENGTH];		// like /dev/hda1
    char grub_name[GRUB_PARTITION_LENGTH];		// like (hd0,0)
    struct partition *next;
};

struct linux_grub_info
{
    char title[STRING_LENGTH];	
    char root[STRING_LENGTH];	
    char kernel_ro[SHORT_STRING_LENGTH] ;	
    char kernel_root[PARTITION_LENGTH];			// like /dev/sda1	
    struct kernel_info *kernel_info_p;
};

struct windows_grub_info
{
    char title[STRING_LENGTH];	
    char rootnoverify[PARTITION_LENGTH];		// like (hd0,0)	
};

struct grub_partition_info
{
    char device_name[PARTITION_LENGTH];        	// like /dev/hda1 
    char device_num;				// start from 0
    int  partition_num;				// start from 0
    char grub_partition_str[PARTITION_LENGTH];	// like (hd0,0) or sth

    union 
    {
	struct linux_grub_info linux_t;
	struct windows_grub_info windows_t;
    }os_t;					// OSs on this partition

    char is_linux;
    char is_windows;
    struct grub_partition_info *next;
};

struct grub_info
{
    char default_boot[SHORT_STRING_LENGTH];
    char timeout[SHORT_STRING_LENGTH];
    char gfxmenu[STRING_LENGTH];
    char splashimage[STRING_LENGTH];
    struct grub_partition_info *grub_partition_info_p;
};

// declaration ahead
int init(void);
int partition_linux_to_grub(const char *str, struct grub_partition_info *p);
int generate_grub_conf_linux(struct grub_partition_info *partition_p, const struct os_info *os_info_p);
int generate_grub_conf_windows(struct grub_partition_info *p, const struct os_info *os_p);
int generate_grub_conf_common(void);
int generate_grub_conf_dt7(struct grub_info *grub_p, const char *title, const char *root_partition, const char *boot_partition);
int write_grub_conf_file(const struct grub_info *grub_p, const char *root_partition, const char *boot_partition);
int generate_grub_conf_file(const char *title, const char *root_partition, const char *boot_partition);
void free_partition_list(struct partition *p);
void free_grub_info(struct grub_info *p);


int set_grub_args(const char *grub_args)
{
	strncpy(dt7_grub_args_sc, grub_args, STRING_LENGTH);
	return 0;
}
int install_grub(const char *title, const char *root_partition, const char *towhere, const char *boot_partition)
{
    char cmd[STRING_LENGTH];

    if (init() != 0)
    {
	fprintf(stderr, "init error\n");
	return -1;
    }

    if (generate_grub_conf_file(title, root_partition, boot_partition) != 0)
    {
	fprintf(stderr, "generate grub.conf.redflag error\n");
	return -1;
    }

    // install grub(location)
    if (towhere == NULL)
    {
	fprintf(stderr, "grub install towhere is NULL\n");
	return -1;
    }

    sprintf(cmd, "%s", grub_install_sc);	
    sprintf(cmd+strlen(grub_install_sc), "%s", towhere);	
    fprintf(stdout, "grub cmd is %s\n", cmd);

    if (system(cmd) == -1)
    {
	fprintf(stderr, "grub install error\n");
	return -1;
    }

    free_partition_list(partition_head_g);

    return 0;
}

// seek every partition to construct partition list
int init(void)
{
    int i,j;
    int partition_num = 0;
    int ide_num = 0;	
    int scsi_num = 0;	
    char *p = NULL;
    char ide[IDE_NUM];
    char scsi[SCSI_NUM];
    PedDevice dev;
    PedDevice *dev_p = NULL;
    PedDisk	 disk ;
    PedDisk	 *disk_p = NULL;
    PedPartition part;
    PedPartition *part_p = NULL;
    struct partition *partition_p = NULL;

    FILE *fp = NULL;
    char buf[1024];
    char *release_start_p = NULL;
    char *release_end_p = NULL;

    if ((fp = fopen("/proc/version", "r")) == NULL) 
    {
	fprintf(stderr, "open /proc/version error\n");
	return false;
    }
    fgets(buf, 1024, fp);
    fclose(fp);

    p = strstr(buf, "Linux version ");

    if (p == NULL)
    {
	fprintf(stderr, "/proc/version' format is error\n");
	return false;
    }

    p += strlen("Linux version ");
    release_start_p = p;
    release_end_p = strchr(release_start_p, ' ');
    *release_end_p = '\0';
    strncpy(dt7_kernel_release_sc, release_start_p, release_end_p-release_start_p+1);
    strncat(dt7_vmlinuz_name_sc, "vmlinuz-", STRING_LENGTH);	
    strncat(dt7_vmlinuz_name_sc, dt7_kernel_release_sc, STRING_LENGTH);	
    strncat(dt7_initrd_name_sc, "initrd-", STRING_LENGTH);	
    strncat(dt7_initrd_name_sc, dt7_kernel_release_sc, STRING_LENGTH);	
    strncat(dt7_initrd_name_sc, ".img", STRING_LENGTH);	

    p = NULL;
    ped_device_probe_all();
    dev_p = ped_device_get_next(dev_p);	// get first device

    if (dev_p == NULL)
    {
	fprintf(stderr, "dev_p is NULL\n");
	return -1;
    }

    while (dev_p != NULL)
    {
	if ((dev_p->type != PED_DEVICE_SCSI) && (dev_p->type != PED_DEVICE_IDE))
	{
	    dev_p = ped_device_get_next(dev_p);
	    continue;
	}

	disk_p = ped_disk_new (dev_p);

	if (disk_p != NULL) 
	{
	    part_p = ped_disk_next_partition(disk_p, part_p);

	    while (part_p != NULL)
	    {
		if ((part_p->num > 0) && (part_p->fs_type != NULL)) 
		{
		    partition_num++;

		    if (partition_num == 1)
		    {
			partition_head_g = (struct partition *)malloc(sizeof(struct partition));
			if (partition_head_g == NULL)
			{
			    fprintf(stderr, "partition_head_g malloc error\n");
			    return -1;
			}

			partition_head_g->next = NULL;
			partition_p = partition_head_g;
		    }
		    else
		    {	
			partition_p->next = (struct partition *)malloc(sizeof(struct partition));
			if (partition_p->next == NULL)
			{
			    fprintf(stderr, "partition_p->next malloc error\n");
			    return -1;
			}
			partition_p->next->next = NULL;
			partition_p = partition_p->next;
		    }	

		    memset(partition_p->dev, '\0', DEV_LENGTH);
		    partition_p->dev_num = 0;
		    partition_p->partition_num = 0;
		    memset(partition_p->partition_name, '\0', PARTITION_LENGTH);
		    memset(partition_p->grub_name, '\0', PARTITION_LENGTH);

		    p = strrchr(dev_p->path, '/');
		    p++;
		    strncat(partition_p->dev, p, DEV_LENGTH);

		    p += 2;
		    partition_p->dev_num = *p;

		    partition_p->partition_num = part_p->num;
		    strncpy(partition_p->partition_name, dev_p->path, PARTITION_LENGTH);
		    sprintf(partition_p->partition_name + strlen(dev_p->path), "%d", partition_p->partition_num);
		}

		part_p = ped_disk_next_partition(disk_p, part_p);
	    }
	}

	dev_p = ped_device_get_next(dev_p);
    }// end while

    for(i=0; i<IDE_NUM; i++)
	ide[i] = 0;

    for(i=0; i<SCSI_NUM; i++)
	scsi[i] = 0;

    partition_p = partition_head_g;

    while (partition_p != NULL)
    {
	if (strncmp(partition_p->dev, "hd", 2) == 0)
	{
	    ide[partition_p->dev_num-'a'] = 1;
	    ide_num++;
	}
	else if (strncmp(partition_p->dev, "sd", 2) == 0)
	{	
	    scsi[partition_p->dev_num-'a'] = 1;
	    scsi_num++;
	}

	partition_p = partition_p->next;
    }// end while

    partition_p = partition_head_g;

    while (partition_p != NULL)
    {
	j = 0;
	if (strncmp(partition_p->dev, "hd", 2) == 0)
	{
	    for (i=0;i<=partition_p->dev_num-'a';i++)
		j++;
	}
	else if (strncmp(partition_p->dev, "sd", 2) == 0)
	{	
	    for (i=0;i<=partition_p->dev_num-'a';i++)
		j++;
	    j += ide_num;
	}

	sprintf(partition_p->grub_name, "(hd");
	sprintf(partition_p->grub_name+3, "%d", j-1);
	sprintf(partition_p->grub_name+4, ",");
	sprintf(partition_p->grub_name+5, "%d", partition_p->partition_num-1);

	if (partition_p->partition_num-1 <= 9)
	    sprintf(partition_p->grub_name+6, ")");
	else if (partition_p->partition_num-1 >= 10)
	    sprintf(partition_p->grub_name+7, ")");
	else if (partition_p->partition_num-1 >= 100)
	    sprintf(partition_p->grub_name+8, ")");

	partition_p = partition_p->next;
    }// end while

    return 0;	
}

// transfer partiton of linux name to grub name
int partition_linux_to_grub(const char *str, struct grub_partition_info *p)
{
    struct partition *partition_p = NULL;

    if (str == NULL)
    {
	fprintf(stderr, "error\n");
	return -1;
    }

    partition_p = partition_head_g;

    while (partition_p != NULL)
    {
	if (strcmp(str, partition_p->partition_name) == 0)	
	{
	    strncpy(p->device_name,	partition_p->partition_name, PARTITION_LENGTH);
	    p->device_num = partition_p->dev_num;
	    p->partition_num = partition_p->partition_num;	
	    strncpy(p->grub_partition_str, partition_p->grub_name, PARTITION_LENGTH);
	    break;
	}

	partition_p = partition_p->next;
    }

    return  (partition_p == NULL)?(-1):(0);
}

int generate_grub_conf_linux(struct grub_partition_info *partition_p, const struct os_info *os_info_p)
{
    struct kernel_info *os_p = NULL;
    struct kernel_info **os_pp = NULL;
    struct linux_grub_info *linux_p = NULL;

    if ((partition_p == NULL) || (os_info_p == NULL))
    {
	fprintf(stderr, "partition_p or os_info_p is NULL\n");
	return -1;
    }

    if (os_info_p->os_num == 0)
	return 0;

    if ((partition_linux_to_grub(os_info_p->device_name, partition_p)) != 0)
    {
	fprintf(stderr, "AApartition_linux_to_grub error\n");
	return -1;	
    }

    linux_p = &(partition_p->os_t.linux_t);

    strncpy(linux_p->title, os_info_p->os_type, STRING_LENGTH);	
    strncpy(linux_p->root, partition_p->grub_partition_str, PARTITION_LENGTH);	

    if (os_info_p->is_boot == 1)
    {
	memset(linux_p->kernel_root, '\0', PARTITION_LENGTH);
    }
    else
	strncpy(linux_p->kernel_root, os_info_p->device_name, PARTITION_LENGTH);		

    strncpy(linux_p->kernel_ro, dt7_kernel_ro_sc, SHORT_STRING_LENGTH);		

    os_p = os_info_p->kernel_info_p;
    os_pp = &(partition_p->os_t.linux_t.kernel_info_p);

    // find in partition list and get grub partition name
    while (os_p != NULL)		
    {
	(*os_pp) = (struct kernel_info *)malloc(sizeof(struct kernel_info));	
	if ((*os_pp) == NULL)
	{
	    fprintf(stderr, "os_pp malloc error\n");
	    return -1;
	}

	(*os_pp)->next = NULL;
	strncpy((*os_pp)->vmlinuz_file_name, os_p->vmlinuz_file_name, BUF_LENGTH_LINE); 
	strncpy((*os_pp)->kernel_release, os_p->kernel_release, BUF_LENGTH_LINE); 
	strncpy((*os_pp)->initrd_file_name, os_p->initrd_file_name, BUF_LENGTH_LINE); 
	os_p = os_p->next;
	os_pp = &((*os_pp)->next);
    }

    return 0;
}

int generate_grub_conf_windows(struct grub_partition_info *p, const struct os_info *os_p)
{
    if ((p == NULL) || (os_p == NULL))
	return -1;

    if ((partition_linux_to_grub(os_p->device_name, p)) != 0)
    {
	fprintf(stderr, "partition_linux_to_grub error\n");
	return -1;	
    }

    strncpy(p->os_t.windows_t.title, os_p->os_type, STRING_LENGTH);	
    strncpy(p->os_t.windows_t.rootnoverify, p->grub_partition_str, PARTITION_LENGTH);		

    return 0;
}

int generate_grub_conf_common(void)
{
    FILE *fp;

    if ( (fp = fopen(GRUB_CONF_FILE, "w+")) == NULL )
    {
	fprintf(stderr, "fopen GRUB_CONF_FILE error ");
	return -1;
    }

    fprintf(fp, "%s\n", "################################################");
    fprintf(fp, "%s\n", "# 		grub.conf");
    fprintf(fp, "%s\n", "# 		It'll be used by grub.");
    fprintf(fp, "%s\n", "# 		You can modify it yourself.");
    fprintf(fp, "%s\n", "# 		Enjoy it! Good luck!");
    fprintf(fp, "%s\n", "################################################");
    fprintf(fp, "\n");

    fclose(fp);
    return 0;
}

int generate_grub_conf_dt7(struct grub_info *grub_p, const char *title, const char *root_partition, const char *boot_partition)
{
    struct kernel_info **kernel_p;
    struct grub_partition_info *p = NULL;
    struct grub_partition_info **pp = NULL;

    // alloc partition for dt7
    pp = &(grub_p->grub_partition_info_p);
    (*pp) =(struct grub_partition_info *)malloc(sizeof(struct grub_partition_info));

    if ((*pp) == NULL)
    {
	fprintf(stderr, "*pp malloc error\n");
	return -1;
    }

    p = grub_p->grub_partition_info_p;
    p->next = NULL;	

    if ((partition_linux_to_grub(boot_partition, p) != 0))
    {
	fprintf(stderr, "BBpartition_linux_to_grub error\n");
	//return -1;	
    }

    p->is_linux = 1; 
    p->is_windows = 0; 
    strncpy(p->os_t.linux_t.title, dt7_title_sc, STRING_LENGTH);
    strncpy(p->os_t.linux_t.root, p->grub_partition_str, PARTITION_LENGTH);
    strncpy(p->os_t.linux_t.kernel_ro, dt7_kernel_ro_sc, SHORT_STRING_LENGTH);
    strncpy(p->os_t.linux_t.kernel_root, root_partition, PARTITION_LENGTH);

    kernel_p = &(p->os_t.linux_t.kernel_info_p);
    (*kernel_p) = (struct kernel_info *)malloc(sizeof(struct kernel_info));
    if ((*kernel_p) == NULL)
    {
	fprintf(stderr, "*kernel_p malloc error\n");
	return -1;
    }

    (*kernel_p)->next = NULL;

    strncpy((*kernel_p)->vmlinuz_file_name, dt7_vmlinuz_name_sc, BUF_LENGTH_LINE);
    strncpy((*kernel_p)->kernel_release, dt7_kernel_release_sc, BUF_LENGTH_LINE);
    strncpy((*kernel_p)->initrd_file_name, dt7_initrd_name_sc, BUF_LENGTH_LINE);

    return 0;
}

int write_grub_conf_file(const struct grub_info *grub_p, const char *root_partition, const char *boot_partition)
{
    FILE *fp;
    struct grub_partition_info *p = NULL;
    struct kernel_info  *kernel_p = NULL;

    if (grub_p == NULL)
    {
	fprintf(stderr, "grub_p is NULL\n");
	return -1;
    }

    if (generate_grub_conf_common() != 0)
    {
	fprintf(stderr, "generate_grub_conf_common error\n");
	return -1;
    }

    if ( (fp = fopen(GRUB_CONF_FILE, "a+")) == NULL )
    {
	fprintf(stderr, "fopen GRUB_CONF_FILE error\n");
	return -1;
    }	

    fprintf(fp, "\n");
    fprintf(fp, "%s", "default=");
    fprintf(fp, "%s\n", grub_p->default_boot);
    fprintf(fp, "%s", "timeout=");
    fprintf(fp, "%s\n", grub_p->timeout);
    fprintf(fp, "%s", "gfxmenu=");
    fprintf(fp, "%s\n", grub_p->gfxmenu);
    fprintf(fp, "%s", "splashimage=");
    fprintf(fp, "%s\n\n", grub_p->splashimage);

    p = grub_p->grub_partition_info_p;
    kernel_p = p->os_t.linux_t.kernel_info_p;

    if (p != NULL)	// write dt7 grub entry
    {	
	if (strcmp(root_partition, boot_partition) == 0)
	{
	    fprintf(fp, "%s", "title  ");
	    fprintf(fp, "%s  ", p->os_t.linux_t.title);
	    fprintf(fp, "(kernel release %s)\n", kernel_p->kernel_release);
	    fprintf(fp, "\troot %s\n", p->os_t.linux_t.root);
	    fprintf(fp, "\tkernel /boot/%s  ", kernel_p->vmlinuz_file_name);
	    fprintf(fp, "%s  ", dt7_kernel_ro_sc); 
	    fprintf(fp, "%s", "vga="); 
	    fprintf(fp, "%s  ", dt7_kernel_vga_sc); 
	    fprintf(fp, "%s", "root="); 
	    fprintf(fp, "%s  ", p->os_t.linux_t.kernel_root);
	    fprintf(fp, "%s  ", dt7_kernel_splash_sc); 
	    fprintf(fp, "%s \n", dt7_grub_args_sc); 
	    fprintf(fp, "\tinitrd /boot/%s\n\n", kernel_p->initrd_file_name);
	}
	else
	{
	    fprintf(fp, "%s", "title  ");
	    fprintf(fp, "%s  ", p->os_t.linux_t.title);
	    fprintf(fp, "(kernel release %s)\n", kernel_p->kernel_release);
	    fprintf(fp, "\troot %s\n", p->os_t.linux_t.root);
	    fprintf(fp, "\tkernel /%s  ", kernel_p->vmlinuz_file_name);
	    fprintf(fp, "%s  ", dt7_kernel_ro_sc); 
	    fprintf(fp, "%s", "vga="); 
	    fprintf(fp, "%s  ", dt7_kernel_vga_sc); 
	    fprintf(fp, "%s", "root="); 
	    fprintf(fp, "%s  ", p->os_t.linux_t.kernel_root);
	    fprintf(fp, "%s  ", dt7_kernel_splash_sc); 
	    fprintf(fp, "%s \n", dt7_grub_args_sc); 
	    fprintf(fp, "\tinitrd /%s\n\n", kernel_p->initrd_file_name);
	}
    }

	if (p != NULL)
	    p = p->next;				
	else
		return 0;

    while(p != NULL)
    {
	if (p->is_linux == 1)
	{
	    kernel_p = p->os_t.linux_t.kernel_info_p;

	    while(kernel_p != NULL)
	    {
		if (strlen(p->os_t.linux_t.kernel_root) != 0)
		{
		    fprintf(fp, "%s", "title  ");
		    fprintf(fp, "%s  ", p->os_t.linux_t.title);
		    fprintf(fp, "(kernel release %s)\n", kernel_p->kernel_release);
		    fprintf(fp, "\troot %s\n", p->os_t.linux_t.root);
		    fprintf(fp, "\tkernel /boot/%s  ", kernel_p->vmlinuz_file_name);
		    fprintf(fp, "%s  ", dt7_kernel_ro_sc); 
		    fprintf(fp, "%s", "vga="); 
		    fprintf(fp, "%s  ", dt7_kernel_vga_sc); 
		    fprintf(fp, "%s", "root="); 
		    fprintf(fp, "%s  ", p->os_t.linux_t.kernel_root);
	//	    fprintf(fp, "%s", "rhgb  "); 
		    fprintf(fp, "%s \n", dt7_kernel_splash_sc); 
		    fprintf(fp, "\tinitrd /boot/%s\n\n", kernel_p->initrd_file_name);
		}
		else
		{	// it is a boot partition
		    fprintf(fp, "%s", "title  ");
		    fprintf(fp, "%s  ", p->os_t.linux_t.title);
		    fprintf(fp, "(kernel release %s)\n", kernel_p->kernel_release);
		    fprintf(fp, "\troot %s\n", p->os_t.linux_t.root);
		    fprintf(fp, "\tkernel /%s  ", kernel_p->vmlinuz_file_name);
		    fprintf(fp, "%s  ", dt7_kernel_ro_sc); 
		    fprintf(fp, "%s", "vga="); 
		    fprintf(fp, "%s  ", dt7_kernel_vga_sc); 
	//	    fprintf(fp, "%s", "rhgb  "); 
		    fprintf(fp, "%s \n", dt7_kernel_splash_sc); 
		    fprintf(fp, "\tinitrd /%s\n\n", kernel_p->initrd_file_name);
		}

		kernel_p = kernel_p->next;
	    }
	}
	else if (p->is_windows == 1)
	{
	    fprintf(fp, "%s", "title  ");
	    fprintf(fp, "%s\n", p->os_t.windows_t.title);
	    fprintf(fp, "\t%s", "rootnoverify ");
	    fprintf(fp, "%s\n", p->os_t.windows_t.rootnoverify);
	    fprintf(fp, "\t%s\n",windows_makeactive_sc);
	    fprintf(fp, "\t%s\n\n",windows_chainloader_sc);
	}

	p = p->next;				
    }

    fclose(fp);
    return 0;

}

// generate a grub.conf file, OK return 0 else -1
int generate_grub_conf_file(const char *title, const char *root_partition, const char *boot_partition)
{
    struct os_info os_info_t;
    struct grub_info grub_info_t;
    struct grub_partition_info *p = NULL;
    struct partition *partition_p = NULL;

    strncpy(grub_info_t.default_boot, dt7_default_boot_sc, SHORT_STRING_LENGTH);	
    strncpy(grub_info_t.timeout, dt7_timeout_sc, SHORT_STRING_LENGTH);	

    generate_grub_conf_dt7(&grub_info_t, title, root_partition, boot_partition);

    //add gfxmenu and splashimage start
    memset(grub_info_t.gfxmenu, '\0', STRING_LENGTH);
    memset(grub_info_t.splashimage, '\0', STRING_LENGTH);

    if (strcmp(root_partition, boot_partition) == 0)
    {
	strncat(grub_info_t.gfxmenu, grub_info_t.grub_partition_info_p->grub_partition_str, STRING_LENGTH);
	strncat(grub_info_t.gfxmenu+strlen(grub_info_t.grub_partition_info_p->grub_partition_str), 	dt7_gfxmenu_sc, STRING_LENGTH);

	strncat(grub_info_t.splashimage, grub_info_t.grub_partition_info_p->grub_partition_str, STRING_LENGTH);
	strncat(grub_info_t.splashimage+strlen(grub_info_t.grub_partition_info_p->grub_partition_str), dt7_splashimage_sc, STRING_LENGTH);
    }
    else
    {
	strncat(grub_info_t.gfxmenu, grub_info_t.grub_partition_info_p->grub_partition_str, STRING_LENGTH);
	strncat(grub_info_t.gfxmenu+strlen(grub_info_t.grub_partition_info_p->grub_partition_str), 	dt7_gfxmenu_boot_sc, STRING_LENGTH);

	strncat(grub_info_t.splashimage, grub_info_t.grub_partition_info_p->grub_partition_str, STRING_LENGTH);
	strncat(grub_info_t.splashimage+strlen(grub_info_t.grub_partition_info_p->grub_partition_str), dt7_splashimage_boot_sc, STRING_LENGTH);
    }

    //add gfxmenu and splashimage end 

    p = grub_info_t.grub_partition_info_p;

    // loop for every partition except boot dir
    partition_p = partition_head_g;

    while(partition_p != NULL)
    {		
	OSIdentify id;

	if ((strcmp(partition_p->partition_name, root_partition) == 0)
		||(strcmp(partition_p->partition_name, boot_partition) == 0))
	{
	    partition_p = partition_p->next;
	    continue;
	}

	if (id.get_os_info(partition_p->partition_name, &os_info_t) != 0)
	{
	    partition_p = partition_p->next;
	    continue;
	}

	struct grub_partition_info *p_tmp = NULL;

	if ((os_info_t.is_linux == 1) || (os_info_t.is_boot == 1))
	{
	    p->next =(struct grub_partition_info *)malloc(sizeof(struct grub_partition_info));
	    if (p->next == NULL)
	    {
		fprintf(stderr, "p->next malloc error\n");
		return -1;
	    }

	    p->next->next = NULL;	
	    p_tmp = p;
	    p = p->next;
	    p->is_linux = 1; 
	    p->is_windows = 0; 

	    if (generate_grub_conf_linux(p, &os_info_t) != 0)
	    {
		fprintf(stderr, "generate grub_conf_linux error\n");
		p = p_tmp;
		free(p->next);
		p->next = NULL;
		partition_p = partition_p->next;
		continue;
	    }
	}
	else if (os_info_t.is_windows == 1)
	{
	    p->next =(struct grub_partition_info *)malloc(sizeof(struct grub_partition_info));
	    if (p->next == NULL)
	    {
		fprintf(stderr, "p->next malloc error\n");
		return -1;
	    }
	    p->next->next = NULL;	
	    p_tmp = p;
	    p = p->next;
	    p->is_linux = 0; 
	    p->is_windows = 1; 

	    if (generate_grub_conf_windows(p, &os_info_t) != 0)
	    {
		fprintf(stderr, "generate grub_conf_windows error\n");
		p = p_tmp;
		free(p->next);
		p->next = NULL;
		partition_p = partition_p->next;
		continue;
	    }
	}

	partition_p = partition_p->next;
    }

    if (write_grub_conf_file(&grub_info_t, root_partition, boot_partition) != 0)
    {
	fprintf(stderr, "write_grub_conf_fiel error\n");
	return -1;
    }

    free_grub_info(&grub_info_t);

    return 0;
}

void free_partition_list(struct partition *p)
{
    struct partition *p_del = p;

    if (p == NULL)
	return;

    while(p!= NULL)
    {
	p = p->next;
	free(p_del);
	p_del = p;	
    }

    return;
}

void free_grub_info(struct grub_info *p)
{
    struct kernel_info *kernel_info_p, *kernel_info_del_p;
    struct grub_partition_info *grub_partition_info_p, *grub_partition_info_del_p;

    if (p == NULL)
	return;

    grub_partition_info_p = p->grub_partition_info_p;
    grub_partition_info_del_p = grub_partition_info_p;

    while(grub_partition_info_p != NULL)
    {
	grub_partition_info_p = grub_partition_info_p->next;

	if (grub_partition_info_del_p->is_linux == 1)
	{
	    kernel_info_p = grub_partition_info_del_p->os_t.linux_t.kernel_info_p;
	    kernel_info_del_p = kernel_info_p;

	    while(kernel_info_p != NULL)
	    {
		kernel_info_p = kernel_info_p->next;
		free(kernel_info_del_p);
		kernel_info_del_p = kernel_info_p;
	    }			
	}

	free(grub_partition_info_del_p);
	grub_partition_info_del_p = grub_partition_info_p;
    }	
}
