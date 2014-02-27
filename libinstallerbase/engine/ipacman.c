#include <stdio.h>
#include <sys/utsname.h> /* uname */
#include <limits.h> /* PATH_MAX */
#include <string.h>
#include <errno.h>

#include "ipacman.h"

#define PACMAN_CALLER_PREFIX "IPACMAN"

static alpm_handle_t *handle = NULL;

/* callback to handle receipt of total download value */
static void cb_dl_total(off_t total)
{
	printf("dl_total=%ld\n", total);
}

/* callback to handle display of download progress */
static void cb_dl_progress(const char *filename, off_t file_xfered, off_t file_total)
{
	printf("downloading %s, %ld, %ld\n", filename, file_xfered, file_total);
}

/* callback to handle messages/notifications from libalpm transactions */
static void cb_event(alpm_event_t event, void *data1, void *data2)
{
	switch(event) {
		case ALPM_EVENT_CHECKDEPS_START:
			printf("checking dependencies...\n");
			break;
		case ALPM_EVENT_FILECONFLICTS_START:
			printf("checking for file conflicts...\n");
			break;
		case ALPM_EVENT_RESOLVEDEPS_START:
			printf("resolving dependencies...\n");
			break;
		case ALPM_EVENT_INTERCONFLICTS_START:
			printf("looking for inter-conflicts...\n");
			break;
		case ALPM_EVENT_ADD_START:
			printf("installing %s...\n", alpm_pkg_get_name(data1));
			break;
		case ALPM_EVENT_ADD_DONE:
			alpm_logaction(handle, PACMAN_CALLER_PREFIX,
					"installed %s (%s)\n",
					alpm_pkg_get_name(data1),
					alpm_pkg_get_version(data1));
			break;
		case ALPM_EVENT_REMOVE_START:
			printf("removing %s...\n", alpm_pkg_get_name(data1));
			break;
		case ALPM_EVENT_REMOVE_DONE:
			alpm_logaction(handle, PACMAN_CALLER_PREFIX,
					"removed %s (%s)\n",
					alpm_pkg_get_name(data1),
					alpm_pkg_get_version(data1));
			break;
		case ALPM_EVENT_UPGRADE_START:
			printf("upgrading %s...\n", alpm_pkg_get_name(data1));
			break;
		case ALPM_EVENT_UPGRADE_DONE:
			alpm_logaction(handle, PACMAN_CALLER_PREFIX,
					"upgraded %s (%s -> %s)\n",
					alpm_pkg_get_name(data1),
					alpm_pkg_get_version(data2),
					alpm_pkg_get_version(data1));
			break;
		case ALPM_EVENT_DOWNGRADE_START:
			printf("downgrading %s...\n", alpm_pkg_get_name(data1));
			break;
		case ALPM_EVENT_DOWNGRADE_DONE:
			alpm_logaction(handle, PACMAN_CALLER_PREFIX,
					"downgraded %s (%s -> %s)\n",
					alpm_pkg_get_name(data1),
					alpm_pkg_get_version(data2),
					alpm_pkg_get_version(data1));
			break;
		case ALPM_EVENT_REINSTALL_START:
			printf("reinstalling %s...\n", alpm_pkg_get_name(data1));
			break;
		case ALPM_EVENT_REINSTALL_DONE:
			alpm_logaction(handle, PACMAN_CALLER_PREFIX,
					"reinstalled %s (%s)\n",
					alpm_pkg_get_name(data1),
					alpm_pkg_get_version(data1));
			break;
		case ALPM_EVENT_INTEGRITY_START:
			printf("checking package integrity...\n");
			break;
		case ALPM_EVENT_KEYRING_START:
			printf("checking keyring...\n");
			break;
		case ALPM_EVENT_KEY_DOWNLOAD_START:
			printf("downloading required keys...\n");
			break;
		case ALPM_EVENT_LOAD_START:
			printf("loading package files...\n");
			break;
		case ALPM_EVENT_DELTA_INTEGRITY_START:
			printf("checking delta integrity...\n");
			break;
		case ALPM_EVENT_DELTA_PATCHES_START:
			printf("applying deltas...\n");
			break;
		case ALPM_EVENT_DELTA_PATCH_START:
			printf("generating %s with %s... ", (char *)data1, (char *)data2);
			break;
		case ALPM_EVENT_DELTA_PATCH_DONE:
			printf("success!\n");
			break;
		case ALPM_EVENT_DELTA_PATCH_FAILED:
			printf("failed.\n");
			break;
		case ALPM_EVENT_SCRIPTLET_INFO:
			fputs((const char *)data1, stdout);
			break;
		case ALPM_EVENT_RETRIEVE_START:
			printf("Retrieving packages ...\n");
			break;
		case ALPM_EVENT_DISKSPACE_START:
			printf("checking available disk space...\n");
			break;
		case ALPM_EVENT_OPTDEP_REQUIRED:
			printf("%s optionally requires %s\n", alpm_pkg_get_name(data1),
					alpm_dep_compute_string(data2));
			break;
		case ALPM_EVENT_DATABASE_MISSING:
			printf("database file for '%s' does not exist\n", (char *)data1);
			break;
			/* all the simple done events, with fallthrough for each */
		case ALPM_EVENT_FILECONFLICTS_DONE:
		case ALPM_EVENT_CHECKDEPS_DONE:
		case ALPM_EVENT_RESOLVEDEPS_DONE:
		case ALPM_EVENT_INTERCONFLICTS_DONE:
		case ALPM_EVENT_INTEGRITY_DONE:
		case ALPM_EVENT_KEYRING_DONE:
		case ALPM_EVENT_KEY_DOWNLOAD_DONE:
		case ALPM_EVENT_LOAD_DONE:
		case ALPM_EVENT_DELTA_INTEGRITY_DONE:
		case ALPM_EVENT_DELTA_PATCHES_DONE:
		case ALPM_EVENT_DISKSPACE_DONE:
			/* nothing */
			break;
	}
	fflush(stdout);
}

/* callback to handle questions from libalpm transactions (yes/no) */
/* TODO this is one of the worst ever functions written. void *data ? wtf */
static void cb_question(alpm_question_t event, void *data1, void *data2,
                   void *data3, int *response)
{
	*response = 1;
}

/* callback to handle display of transaction progress */
static void cb_progress(alpm_progress_t event, const char *pkgname, int percent,
                       size_t howmany, size_t current)
{
	printf("pkgname=%s, percent=%d, howmany=%ld, current=%ld\n", pkgname, percent, howmany, current);
}

static int trans_release(alpm_handle_t *handle)
{
	if(alpm_trans_release(handle) == -1) {
		printf("failed to release transaction (%s)\n", alpm_strerror(alpm_errno(handle)));
		return -1;
	}
	return 0;
}

static void trans_init_error(alpm_handle_t *handle)
{
	alpm_errno_t err = alpm_errno(handle);
	printf("failed to init transaction (%s)\n", alpm_strerror(err));
	if(err == ALPM_ERR_HANDLE_LOCK) {
		const char *lockfile = alpm_option_get_lockfile(handle);
		printf("could not lock database: %s\n", strerror(errno));
		if(access(lockfile, F_OK) == 0) {
			fprintf(stderr, "  if you're sure a package manager is not already\n"
					"  running, you can remove %s\n", lockfile);
		}
	}
}

static alpm_db_t *get_db(const char *dbname)
{
	alpm_list_t *i;
	for(i = alpm_get_syncdbs(handle); i; i = i->next) {
		alpm_db_t *db = i->data;
		if(strcmp(alpm_db_get_name(db), dbname) == 0) {
			return db;
		}
	}
	return NULL;
}

static int process_pkg(alpm_pkg_t *pkg)
{
	int ret = alpm_add_pkg(handle, pkg);

	if(ret == -1) {
		alpm_errno_t err = alpm_errno(handle);
		if(err == ALPM_ERR_TRANS_DUP_TARGET
				|| err == ALPM_ERR_PKG_IGNORED) {
			/* just skip duplicate or ignored targets */
			printf("skipping target: %s\n", alpm_pkg_get_name(pkg));
			return 0;
		} else {
			printf("'%s': %s\n", alpm_pkg_get_name(pkg),
					alpm_strerror(err));
			return 1;
		}
	}
	return 0;
}

static int process_group(alpm_list_t *dbs, const char *group, int error)
{
	int ret = 0;
	alpm_list_t *i;
	alpm_list_t *pkgs = alpm_find_group_pkgs(dbs, group);
	int count = alpm_list_count(pkgs);

	if(!count) {
		printf("target not found: %s\n", group);
		return 1;
	}

	if(error) {
		/* we already know another target errored. there is no reason to prompt the
		 * user here; we already validated the group name so just move on since we
		 * won't actually be installing anything anyway. */
		goto cleanup;
	}

	for(i = pkgs; i; i = alpm_list_next(i)) {
		alpm_pkg_t *pkg = i->data;

		if(process_pkg(pkg) == 1) {
			ret = 1;
			goto cleanup;
		}
	}

cleanup:
	alpm_list_free(pkgs);
	return ret;
}

static int process_targname(alpm_list_t *dblist, const char *targname,
		int error)
{
	alpm_pkg_t *pkg = alpm_find_dbs_satisfier(handle, dblist, targname);

	/* #FS#23342 - skip ignored packages when user says no */
	if(alpm_errno(handle) == ALPM_ERR_PKG_IGNORED) {
			printf("skipping target: %s\n", targname);
			return 0;
	}

	if(pkg) {
		return process_pkg(pkg);
	}
	return process_group(dblist, targname, error);
}


static int process_target(const char *target, int error)
{
	/* process targets */
	char *targstring = strdup(target);
	char *targname = strchr(targstring, '/');
	int ret = 0;
	alpm_list_t *dblist;

	if(targname && targname != targstring) {
		alpm_db_t *db;
		const char *dbname;

		*targname = '\0';
		targname++;
		dbname = targstring;
		db = get_db(dbname);
		if(!db) {
			printf("database not found: %s\n", dbname);
			ret = 1;
			goto cleanup;
		}
		dblist = alpm_list_add(NULL, db);
		ret = process_targname(dblist, targname, error);
		alpm_list_free(dblist);
	} else {
		targname = targstring;
		dblist = alpm_get_syncdbs(handle);
		ret = process_targname(dblist, targname, error);
	}

cleanup:
	free(targstring);
	if(ret && access(target, R_OK) == 0) {
		printf("'%s' is a file, did you mean %s instead of %s?\n",
				target, "-U/--upgrade", "-S/--sync");
	}
	return ret;
}

static int sync_prepare_execute(void)
{
	alpm_list_t *i, *packages, *data = NULL;
	int retval = 0;

	/* Step 2: "compute" the transaction based on targets and flags */
	if(alpm_trans_prepare(handle, &data) == -1) {
		alpm_errno_t err = alpm_errno(handle);
		printf("failed to prepare transaction (%s)\n", alpm_strerror(err));
		switch(err) {
			case ALPM_ERR_PKG_INVALID_ARCH:
				for(i = data; i; i = alpm_list_next(i)) {
					const char *pkg = i->data;
					printf("package %s does not have a valid architecture\n", pkg);
				}
				break;
			case ALPM_ERR_UNSATISFIED_DEPS:
				for(i = data; i; i = alpm_list_next(i)) {
					alpm_depmissing_t *miss = i->data;
					char *depstring = alpm_dep_compute_string(miss->depend);
					printf("%s: requires %s\n", miss->target, depstring);
					free(depstring);
				}
				break;
			case ALPM_ERR_CONFLICTING_DEPS:
				for(i = data; i; i = alpm_list_next(i)) {
					alpm_conflict_t *conflict = i->data;
					/* only print reason if it contains new information */
					if(conflict->reason->mod == ALPM_DEP_MOD_ANY) {
						printf("%s and %s are in conflict\n",
								conflict->package1, conflict->package2);
					} else {
						char *reason = alpm_dep_compute_string(conflict->reason);
						printf("%s and %s are in conflict (%s)\n",
								conflict->package1, conflict->package2, reason);
						free(reason);
					}
				}
				break;
			default:
				break;
		}
		retval = 1;
		goto cleanup;
	}

	packages = alpm_trans_get_add(handle);
	if(packages == NULL) {
		/* nothing to do: just exit without complaining */
			printf(" there is nothing to do\n");
		goto cleanup;
	}

	printf("\n");

	if(alpm_trans_commit(handle, &data) == -1) {
		alpm_errno_t err = alpm_errno(handle);
		printf("failed to commit transaction (%s)\n",
		        alpm_strerror(err));
		switch(err) {
			case ALPM_ERR_FILE_CONFLICTS:
				printf("unable to %s directory-file conflicts\n", "--force");
				for(i = data; i; i = alpm_list_next(i)) {
					alpm_fileconflict_t *conflict = i->data;
					switch(conflict->type) {
						case ALPM_FILECONFLICT_TARGET:
							printf("%s exists in both '%s' and '%s'\n",
									conflict->file, conflict->target, conflict->ctarget);
							break;
						case ALPM_FILECONFLICT_FILESYSTEM:
							printf("%s: %s exists in filesystem\n",
									conflict->target, conflict->file);
							break;
					}
				}
				break;
			case ALPM_ERR_PKG_INVALID:
			case ALPM_ERR_PKG_INVALID_CHECKSUM:
			case ALPM_ERR_PKG_INVALID_SIG:
			case ALPM_ERR_DLT_INVALID:
				for(i = data; i; i = alpm_list_next(i)) {
					const char *filename = i->data;
					printf("%s is invalid or corrupted\n", filename);
				}
				break;
			default:
				break;
		}
		/* TODO: stderr? */
		printf("Errors occurred, no packages were upgraded.\n");
		retval = 1;
		goto cleanup;
	}

	/* Step 4: release transaction resources */
cleanup:
	if(data) {
		FREELIST(data);
	}
	if(trans_release(handle) == -1) {
		retval = 1;
	}

	return retval;
}

static int mkdirp(const char *pathname, mode_t mode)
{
	char parent[PATH_MAX], *p;
	/* make a parent directory path */
	strncpy(parent, pathname, sizeof(parent));
	parent[sizeof(parent) - 1] = '\0';
	for(p = parent + strlen(parent); *p != '/' && p != parent; p--);
	*p = '\0';
	/* try make parent directory */
	if(p != parent && mkdirp(parent, mode) != 0)
		return -1;
	/* make this one if parent has been made */
	if(mkdir(pathname, mode) == 0)
		return 0;
	/* if it already exists that is fine */
	if(errno == EEXIST)
		return 0;
	return -1;
}

int ipacman_init(const char* rootdir, alpm_cb_progress cb)
{
	int ret = 0;
	alpm_errno_t err;

	/* Configure root path first. If it is set and dbpath/logfile were not
	 * set, then set those as well to reside under the root. */
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/var/lib/pacman/", rootdir);

	if(access(path, F_OK) != 0) {
		mkdirp(path, 0755);
	}

	/* initialize library */
	handle = alpm_initialize(rootdir, path, &err);
	if(!handle) {
		printf("failed to initialize alpm library (%s)\n", alpm_strerror(err));
		if(err == ALPM_ERR_DB_VERSION) {
			printf("  try running pacman-db-upgrade\n");
		}
		return -1;
	}

	alpm_option_set_dlcb(handle, cb_dl_progress);
	alpm_option_set_eventcb(handle, cb_event);
	alpm_option_set_questioncb(handle, cb_question);
	alpm_option_set_progresscb(handle, cb?cb:cb_progress);

	snprintf(path, PATH_MAX, "%s/var/log/pacman.log", rootdir);
	ret = alpm_option_set_logfile(handle, path);
	if(ret != 0) {
		printf("problem setting logfile '%s' (%s)\n", path, alpm_strerror(alpm_errno(handle)));
		return ret;
	}

	/* Set GnuPG's home directory.  This is not relative to rootdir, even if
	 * rootdir is defined. Reasoning: gpgdir contains configuration data. */
	snprintf(path, PATH_MAX, "%s/etc/pacman.d/gnupg/", rootdir);
	ret = alpm_option_set_gpgdir(handle, path);
	if(ret != 0) {
		printf("problem setting gpgdir '%s' (%s)\n", path, alpm_strerror(alpm_errno(handle)));
		return ret;
	}

	/* add a default cachedir if one wasn't specified */
	snprintf(path, PATH_MAX, "%s/var/cache/pacman/pkg/", rootdir);
	alpm_option_add_cachedir(handle, path);
    //specialized for installer
	alpm_option_add_cachedir(handle, "/PKGS");

	alpm_option_set_default_siglevel(handle, ALPM_SIG_PACKAGE | ALPM_SIG_PACKAGE_OPTIONAL |
			ALPM_SIG_DATABASE | ALPM_SIG_DATABASE_OPTIONAL);


	alpm_option_set_local_file_siglevel(handle, ALPM_SIG_USE_DEFAULT);
	alpm_option_set_remote_file_siglevel(handle, ALPM_SIG_USE_DEFAULT);

	alpm_option_set_totaldlcb(handle, cb_dl_total);

	struct utsname un;
	uname(&un);
	alpm_option_set_arch(handle, un.machine);
	alpm_option_set_checkspace(handle, 1);
	alpm_option_set_usesyslog(handle, 1);
	alpm_option_set_deltaratio(handle, 0.7);

	/* the follow function use alpm_list_t* as arguments
	 * use alpm_list_add()
	 */
	/*
	alpm_option_set_ignorepkgs(handle, ignorepkg);
	alpm_option_set_ignoregroups(handle, ignoregrp);
	alpm_option_set_noupgrades(handle, noupgrade);
	alpm_option_set_noextracts(handle, noextract);
	*/
	return 0;
}


/** Free the resources.
 *
 * @param ret the return value
 */
int ipacman_add_server(const char *name, const char *server)
{
	alpm_db_t *db;
	db = alpm_register_syncdb(handle, name, ALPM_SIG_DATABASE_OPTIONAL);
	if(db == NULL) {
		printf("could not register '%s' database (%s)\n", name, alpm_strerror(alpm_errno(handle)));
		return 1;
	}
	if(alpm_db_add_server(db, server) != 0) {
		printf("could not add server URL to database '%s': %s (%s)\n",
				name, server, alpm_strerror(alpm_errno(handle)));
		return 1;
	}
	return 0;
}

int ipacman_refresh_databases(void)
{
	alpm_list_t *sync_dbs = NULL;
	alpm_list_t *i;
	unsigned int success = 0;

	sync_dbs = alpm_get_syncdbs(handle);
	if(sync_dbs == NULL) {
		printf("no usable package repositories configured.\n");
		return -1;
	}

	printf("Synchronizing package databases...\n");
	alpm_logaction(handle, PACMAN_CALLER_PREFIX, "synchronizing package lists\n");

	for(i = sync_dbs; i; i = alpm_list_next(i)) {
		alpm_db_t *db = i->data;

		int ret = alpm_db_update(1, db);
		if(ret < 0) {
			printf("failed to update %s (%s)\n",
					alpm_db_get_name(db), alpm_strerror(alpm_errno(handle)));
		} else if(ret == 1) {
			printf(" %s is up to date\n", alpm_db_get_name(db));
			success++;
		} else {
			success++;
		}
	}

	/* We should always succeed if at least one DB was upgraded - we may possibly
	 * fail later with unresolved deps, but that should be rare, and would be
	 * expected
	 */
	if(!success) {
		printf("failed to synchronize any databases\n");
		trans_init_error(handle);
		return success;
	}
	return 0;
}

/*
 * Return: 0 for success.
 */
int ipacman_sync_packages(alpm_list_t *targets)
{
	int ret = 0;
	alpm_list_t *i;
	alpm_list_t *sync_dbs = alpm_get_syncdbs(handle);

	if(sync_dbs == NULL) {
		printf("no usable package repositories configured.\n");
		return 1;
	}

	/* ensure all known dbs are valid */
	for(i = sync_dbs; i; i = alpm_list_next(i)) {
		alpm_db_t *db = i->data;
		if(alpm_db_get_valid(db)) {
			printf("database '%s' is not valid (%s)\n",
					alpm_db_get_name(db), alpm_strerror(alpm_errno(handle)));
			ret = 1;
		}
	}
	if (ret)
		return ret;

	/* Step 1: create a new transaction... */
	ret = alpm_trans_init(handle, ALPM_TRANS_FLAG_FORCE);
	if(ret == -1) {
		trans_init_error(handle);
		return 1;
	}

	/* process targets */
	for(i = targets; i; i = alpm_list_next(i)) {
		const char *targ = i->data;
		if(process_target(targ, ret) == 1) {
			ret = 1;
		}
	}

	if(ret) {
		if(alpm_trans_release(handle) == -1) {
			printf("failed to release transaction (%s)\n", alpm_strerror(alpm_errno(handle)));
		}
		return ret;
	}

	return sync_prepare_execute();
}

void ipacman_cleanup(void)
{
    //specialized for installer
    alpm_option_remove_cachedir(handle, "/PKGS");

	/* free alpm library resources */
	if(handle && alpm_release(handle) == -1) {
		printf("error releasing alpm library\n");
	}
}
