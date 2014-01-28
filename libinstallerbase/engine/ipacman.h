/* vi: set sw=4 ts=4 wrap ai: */
/*
 * ipacman.h: This file is part of ____
 *
 * Copyright (C) 2014 yetist <xiaotian.wu@i-soft.com.cn>
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * */

#ifndef __IPACMAN_H__ 
#define __IPACMAN_H__  1

#include <alpm.h>
#include <alpm_list.h>

__BEGIN_DECLS
//extern "C" {
int ipacman_init(const char* rootdir, alpm_cb_progress cb);
int ipacman_add_server(const char *name, const char *server);
int ipacman_refresh_databases(void);
int ipacman_sync_packages(alpm_list_t *targets);
void ipacman_cleanup(void);

__END_DECLS
//}

#endif /* __IPACMAN_H__ */
