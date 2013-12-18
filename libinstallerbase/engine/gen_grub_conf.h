#ifndef LIBINSTALLERBASE_ENGINE_GEN_GRUB_CONF_H_
#define LIBINSTALLERBASE_ENGINE_GEN_GRUB_CONF_H_

int set_grub_args(const char *grub_args);
/* 
 * install grub
 * params:
 *     title: user defined title which as name of grub entry.
 * return:
 *     0 if success, -1 if failed.
 */
int install_grub(const char *title, const char *root_partition, const char *towhere, const char *boot_partition);
int install_grub2(const char *title, const char *root_partition, const char *towhere, const char *boot_partition);

#endif //LIBINSTALLERBASE_ENGINE_GEN_GRUB_CONF_H_
