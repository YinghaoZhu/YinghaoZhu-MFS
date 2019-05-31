#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include "root.h"
#include "block.h"
#include "inode.h"
#include "dir.h"
#include "log.h"

void usage()
{
    puts("Usage: mfs -v -bn block_number -bs block_size mount_point");
}

int mfs_rename(const char *old_path, const char *new_path)
{
    inode_t *old_dir, *old_inode;
    inode_t *new_dir, *new_inode;
    char old_base_name[MAX_BASE_NAME];
    char new_base_name[MAX_BASE_NAME];

    log_printf("rename(%s, %s)\n", old_path, new_path);
    new_dir = resolv_part_path(new_path, new_base_name);
    if (!new_dir)
        return -ENOENT;
    new_inode = dir_lookup(new_dir, new_base_name);
    if (new_inode)
        return -ENOSYS;

    old_dir = resolv_part_path(old_path, old_base_name);
    if (!old_dir)
        return -ENOENT;
    old_inode = dir_lookup(old_dir, old_base_name);
    if (!old_inode)
        return -ENOENT;

    dir_remove_entry(old_dir, old_base_name);
    dir_add_entry(new_dir, new_base_name, old_inode);
    return 0;
}

int mfs_chmod(const char *path, mode_t mode)
{
    log_printf("chmod(%s, %o)\n", path, mode);
    return 0;
}

int mfs_access(const char *path, int mask)
{
    log_printf("access(%s, %o)\n", path, mask);
    return 0;
}

int mfs_create(const char *path, mode_t mode, struct fuse_file_info *info)
{
    inode_t *dir;
    inode_t *inode;
    char base_name[MAX_BASE_NAME];
    int flags = info->flags;

    log_printf("creat(%s, %o)\n", path, mode);
    dir = resolv_part_path(path, base_name);
    if (!dir) {
        log_error("creat(%s): no such file\n", path);
        return -ENOENT; 
    }

    if (!inode_is_dir(dir)) {
        log_error("creat(%s): not directory\n", path);
        return -ENOTDIR;
    }

    inode = dir_lookup(dir, base_name);
    if (inode) {
        log_error("creat(%s): already exists\n", path);
        return -EEXIST;
    }

    inode = inode_new();
    dir_add_entry(dir, base_name, inode);
    info->fh = (intptr_t)inode;
    return 0;
}

int mfs_open(const char *path, struct fuse_file_info *info)
{
    inode_t *inode;
    int flags = info->flags;

    log_printf("open %s:%p\n", path, info);
    inode = resolv_full_path(path);
    if (!inode) {
        log_error("open (%s): no such file\n", path);
        return -ENOENT;
    }
    if (inode_is_dir(inode)) {
        log_error("open (%s): is directory\n", path);
        return -EISDIR;
    }

    info->fh = (intptr_t)inode;
    return 0;
}

int mfs_release(const char *path, struct fuse_file_info *info)
{
    log_printf("release %s\n", path);
    info->fh = 0;
    return 0;
}

int mfs_read(const char *path, char *buf, size_t size, off_t offset,
             struct fuse_file_info *info)
{
    int error;
    inode_t *inode;

    log_printf("read(%s, %d) at %jd\n", path, size, offset);
    inode = (inode_t *)(intptr_t)(info->fh);
    inode_check(inode);
    error = inode_read(inode, offset, buf, size);
    return error;
}

int mfs_write(const char *path, const char *buf, size_t size, off_t offset,
              struct fuse_file_info *info)
{
    int error;
    inode_t *inode;

    log_printf("write(%s, %d) at %jd\n", path, size, offset);
    inode = (inode_t *)(intptr_t)(info->fh);
    inode_check(inode);
    error = inode_write(inode, offset, buf, size);
    return error;
}

int mfs_mknod(const char *path, mode_t mode, dev_t dev)
{
    inode_t *dir;
    inode_t *inode;
    char base_name[MAX_BASE_NAME];

    log_printf("mknod %s\n", path);
    if (!S_ISREG(mode))
        return -ENOSYS;

    dir = resolv_part_path(path, base_name);
    if (!dir) {
        log_error("mknod(%s): no such file\n", path);
        return -ENOENT; 
    }

    if (!inode_is_dir(dir)) {
        log_error("mknod(%s): not directory\n", path);
        return -ENOTDIR;
    }

    inode = dir_lookup(dir, base_name);
    if (inode) {
        log_error("mknod(%s): already exists\n", path);
        return -EEXIST;
    }

    inode = inode_new();
    dir_add_entry(dir, base_name, inode);
    return 0;
}

int mfs_unlink(const char *path)
{
    inode_t *dir;
    inode_t *inode;
    char base_name[MAX_BASE_NAME];

    log_printf("unlink(%s)\n", path);
    dir = resolv_part_path(path, base_name);
    if (!dir) {
        log_error("unlink(%s): no such file\n", path);
        return -ENOENT; 
    }

    if (!inode_is_dir(dir)) {
        log_error("unlink(%s): not directory\n", path);
        return -ENOTDIR;
    }

    inode = dir_lookup(dir, base_name);
    if (!inode) {
        log_error("unlink(%s): no such file\n", path);
        return -ENOENT;
    }

    if (inode_is_dir(inode)) {
        log_error("unlink(%s): is directory\n", path);
        return -EISDIR;
    }

    dir_remove_entry(dir, base_name);
    inode_delete(inode);
    return 0;
}

int mfs_mkdir(const char *path, mode_t mode)
{
    inode_t *dir;
    inode_t *inode;
    char base_name[MAX_BASE_NAME];

    log_printf("mkdir(%s, %o)\n", path, mode);
    dir = resolv_part_path(path, base_name);
    if (!dir) {
        log_error("mkdir(%s): no such file\n", path);
        return -ENOENT; 
    }

    if (!inode_is_dir(dir)) {
        log_error("mkdir(%s): not directory\n", path);
        return -ENOTDIR;
    }

    if (dir_lookup(dir, base_name)) {
        log_error("mkdir(%s): already exists\n", path);
        return -EEXIST;
    }

    inode = dir_new();
    dir_add_entry(dir, base_name, inode);
    return 0;
}

int mfs_rmdir(const char *path)
{
    inode_t *dir;
    inode_t *inode;
    char base_name[MAX_BASE_NAME];

    log_printf("rmdir(%s)\n", path);
    dir = resolv_part_path(path, base_name);
    if (!dir) {
        log_error("rmdir(%s): no such file\n", path);
        return -ENOENT; 
    }

    if (!inode_is_dir(dir)) {
        log_error("rmdir(%s): not directory\n", path);
        return -ENOTDIR;
    }

    inode = dir_lookup(dir, base_name);
    if (!inode) {
        log_error("rmdir(%s): no such file\n", path);
        return -ENOENT;
    }

    if (!inode_is_dir(inode)) {
        log_error("rmdir(%s): not directory\n", path);
        return -ENOTDIR;
    }

    if (!dir_is_empty(inode)) {
        log_error("rmdir(%s): not empty\n", path);
        return -ENOTEMPTY;
    }

    dir_remove_entry(dir, base_name);
    return 0;
}

int mfs_opendir(const char *path, struct fuse_file_info *info)
{
    inode_t *inode;

    log_printf("opendir %s\n", path);
    inode = resolv_full_path(path);
    if (!inode) {
        log_error("opendir %s: no such file\n", path);
        return -ENOENT;
    }

    if (!inode_is_dir(inode)) {
        log_error("opendir %s: not directory\n", path);
        return -ENOTDIR;
    }
    
    info->fh = (intptr_t)inode;
    return 0;
}

int mfs_releasedir(const char *path, struct fuse_file_info *info)
{
    log_printf("releasedir %s\n", path);
    info->fh = 0;
    return 0;
}

int mfs_readdir(const char *path, void *buf, fuse_fill_dir_t fill, 
                  off_t offset, struct fuse_file_info *info)
{
    inode_t *dir;
    dir_entry_t *dir_entry;
    chain_t *chain;

    log_printf("readdir %s\n", path);
    dir = (inode_t *)(intptr_t)(info->fh);
    for_each (chain, dir->dir_entry_list) {
        dir_entry = host_of(dir_entry_t, chain, chain);
	if (fill(buf, dir_entry->name, NULL, 0) != 0) {
            log_error("readdir %s: fill failed\n", path);
            return -ENOMEM;
        }
    }
    return 0;
}

int mfs_getattr(const char *path, struct stat *buf)
{
    int error;
    inode_t *inode;

    log_printf("getattr %s\n", path);
    inode = resolv_full_path(path);
    if (!inode) {
        log_error("getattr %s: no such file\n", path);
        return -ENOENT;
    }
    error = inode_stat(inode, buf);
    return error;
}

void mfs_destroy(void *userdata)
{
    log_puts("mfs_destroy");
    log_close();
}

struct fuse_operations mfs_operations = {
    .rename = mfs_rename,
    .chmod = mfs_chmod,
    .access = mfs_access,

    .mkdir = mfs_mkdir,
    .rmdir = mfs_rmdir,
    .opendir = mfs_opendir,
    .releasedir = mfs_releasedir,
    .readdir = mfs_readdir,

    .open = mfs_open,
    .create = mfs_create,
    .mknod = mfs_mknod,
    .unlink = mfs_unlink,
    .release = mfs_release,
    .read = mfs_read,
    .write = mfs_write,

    .getattr = mfs_getattr,
    .destroy = mfs_destroy
};

int compress_args(int argc, char *argv[])
{
    int i;
    int j;

    for (i = 0, j = 0; i < argc; i++) {
        char *arg = argv[i];
        if (arg != NULL) {
            argv[j] = arg;
            j++;
        }
    }
    return j;
}

int main(int argc, char *argv[])
{
    int i;

    if (argc == 1) {
        usage();
        return 0;
    }

    total_block_number = 1024;
    block_size = 1024;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            log_verbose = 1;
            argv[i] = NULL;
            continue;
        }

        if (strcmp(argv[i], "-bs") == 0) {
            argv[i] = NULL;
            i++;
            block_size = atoi(argv[i]);
            argv[i] = NULL;
            continue;
        }

        if (strcmp(argv[i], "-bn") == 0) {
            argv[i] = NULL;
            i++;
            total_block_number = atoi(argv[i]);
            argv[i] = NULL;
            continue;
        }
    }
    argc = compress_args(argc, argv);

    log_open();
    blocks_init();
    root_dir_init();
    return fuse_main(argc, argv, &mfs_operations, NULL);
}
