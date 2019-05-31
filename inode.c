#include "block.h"
#include "inode.h"
#include "root.h"

int inode_is_dir(inode_t *this)
{
    return this->type == INODE_DIR;
}

int inode_is_regular(inode_t *this)
{
    return this->type == INODE_REGULAR;
}

void inode_check(inode_t *this)
{
    if (this->magic != INODE_MAGIC)
        log_panic("inode_check failed!\n");
}

inode_t *inode_new()
{
    int i;
    inode_t *this = new(inode_t);

    this->magic = INODE_MAGIC;
    this->type = INODE_REGULAR;
    this->size = 0;
    this->index_block = alloc_block();
    for (i = 0; i < entries_per_block; i++)
        this->index_block[i] = NULL;
    return this;
}

void inode_delete(inode_t *this)
{
    int i;

    for (i = 0; i < entries_per_block; i++) {
        void *block = this->index_block[i];
        if (block)
            free_block(block);
    }

    delete(this);
}

inode_t *inode_make(char *content)
{
    inode_t *this = inode_new();
    int len = strlen(content);

    inode_write(this, 0, content, len);
    return this;
}

void *inode_read_map(inode_t *this, int logic_block)
{
    int block_number = block_number_of(this->size);

    assert(this->type == INODE_REGULAR);
    if (logic_block >= block_number)
        return NULL;
    return this->index_block[logic_block];
}

void *inode_write_map(inode_t *this, int logic_block)
{
    int block_number = block_number_of(this->size);
    void **slot;

    assert(this->type == INODE_REGULAR);
    if(logic_block >= block_number)
        return NULL;
    slot = this->index_block + logic_block;
    if (*slot == NULL)
        *slot = alloc_block();
    return *slot;
}


void inode_expand(inode_t *this, int new_size)
{
    int old_end;
    int new_end;
    int i;

    assert(this->type == INODE_REGULAR);
    if (this->size >= new_size)
        return;

    old_end = block_number_of(this->size);
    new_end = block_number_of(new_size);
    for (i = old_end; i < new_end; i++)
        this->index_block[i] = NULL;
    this->size = new_size;
}
int inode_read(inode_t *this, off_t pos, void *buf, size_t size)
{
    off_t start = pos;
    off_t end = min(start + size, this->size);

    while (pos < end) {
        int div = pos / block_size;
        int mod = pos % block_size;

        void *block = inode_read_map(this, div);
        int  chunk = min(block_size - mod, end - pos);
        memcpy(buf, block + mod, chunk);

        pos += chunk;
        buf += chunk;
    }
    return pos - start;
}

int inode_write(inode_t *this, off_t pos, const void *buf, size_t size)
{
    off_t start = pos;
    off_t end = max(start + size, this->size);

    inode_expand(this, end);
    while (pos < end) {
        int div = pos / block_size;
        int mod = pos % block_size; 

        void *block = inode_write_map(this, div);
        int chunk = min(block_size - mod, end - pos); 
        memcpy(block + mod, buf, chunk); 

        pos += chunk;
        buf += chunk;
    }
    return pos - start;
}

#ifdef notyet
struct stat{
   ev_t     st_dev;     /* ID of device containing file */
   ino_t     st_ino;     /* inode number */
   mode_t    st_mode;    /* protection */
   nlink_t   st_nlink;   /* number of hard links */
   uid_t     st_uid;     /* user ID of owner */
   gid_t     st_gid;     /* group ID of owner */
   dev_t     st_rdev;    /* device ID (if special file) */
   off_t     st_size;    /* total size, in bytes */
   blksize_t st_blksize; /* blocksize for file systemI/O */
   blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
   time_t    st_atime;   /* time of last access*/
   time_t    st_mtime;   /* time of last modification */
   time_t    st_ctime;   /* time of laststatus change */
};
#endif

int inode_stat(inode_t *this, struct stat *buf)
{
    int type;
    int perm = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    
    switch (this->type) {
        case INODE_REGULAR:
            type = S_IFREG;
            break;
            
        case INODE_DIR:
            type = S_IFDIR;
            break;
    }
    
    buf->st_dev = 0;
    buf->st_ino = 0;
    buf->st_mode = type | perm;
    buf->st_nlink = 0;
    
    buf->st_uid = getuid();
    buf->st_gid = getgid();
    buf->st_rdev = 0;
    buf->st_size = this->size;
    buf->st_blksize = 512;
    buf->st_blocks = 0;
    
    buf->st_atime = 0;
    buf->st_mtime = 0;
    buf->st_ctime = 0;
    return 0;
}
