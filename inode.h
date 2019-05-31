#ifndef _INODE_H
#define _INODE_H

#include "list.h"
#include <sys/stat.h>

enum {
    INODE_REGULAR = 0x1356DEAB,
    INODE_DIR,
};

#define INODE_MAGIC 0xDAEA1562
typedef struct {
    int magic;
    int type;
    int size;

    union {
        list_t dir_entry_list;
        void **index_block;
    };
} inode_t;

extern inode_t *inode_new();
extern void inode_delete(inode_t *this);
extern inode_t *inode_make(char *content);
extern int inode_read(inode_t *this, off_t offset, void *buf, size_t size);
extern int inode_write(inode_t *this, off_t offset, const void *buf, size_t size);
extern int inode_stat(inode_t *this, struct stat *buf);

extern int inode_is_dir(inode_t *this);
extern int inode_is_regular(inode_t *this);
extern void inode_check(inode_t *this);

#endif

