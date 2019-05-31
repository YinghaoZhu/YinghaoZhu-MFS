#ifndef _DIR_H
#define _DIR_H

#define MAX_PATH_NAME 256
#define MAX_PATH_COMPONENTS 16
#define MAX_BASE_NAME 32

#include "inode.h"

typedef struct {
    chain_t chain;
    char name[MAX_BASE_NAME];
    inode_t *inode;
} dir_entry_t;

extern inode_t *dir_new();
extern int dir_is_empty(inode_t *this);
extern inode_t *dir_lookup(inode_t *this, char *name);
extern void dir_add_entry(inode_t *this, char *name, inode_t *inode);
extern void dir_remove_entry(inode_t *this, char *name);

enum {
    RESOLV_FULL,
    RESOLV_PART,
};

extern inode_t *resolv_path(int flag, const char *path, char *base_name);
extern inode_t *resolv_full_path(const char *path);
extern inode_t *resolv_part_path(const char *path, char *name);

extern inode_t *root_dir;
extern void root_dir_init();

#endif
