#include"root.h"
#include"dir.h"

inode_t *root_dir;

void dir_add_stuff(inode_t *dir)
{
    inode_t *inode;

    inode = inode_make("HEllO\n");
    dir_add_entry(dir, "hello", inode);

    inode = inode_make("WORLD\n");
    dir_add_entry(dir, "world", inode);
}

void root_dir_init()
{
    inode_t *sub_dir;

    root_dir = dir_new();
    dir_add_stuff(root_dir);

    sub_dir = dir_new();
    dir_add_stuff(sub_dir);
    dir_add_entry(root_dir, "dir", sub_dir);
}

int dir_is_empty(inode_t *this)
{
    int sum = 0;
    chain_t *chain;

    for_each(chain, this->dir_entry_list)
        sum++;
    return sum == 2;
}

inode_t *dir_lookup(inode_t *this, char *name)
{
    chain_t *chain;    

    for_each (chain, this->dir_entry_list) {
        dir_entry_t *dir_entry = host_of(dir_entry_t, chain, chain);
        if (strcmp(dir_entry->name, name) == 0)
            return dir_entry->inode;
    }
    return NULL;
}

dir_entry_t *dir_entry_new(char *name, inode_t *inode)
{
    dir_entry_t *this = new(dir_entry_t);
    chain_init(&this->chain);
    strcpy(this->name, name);
    this->inode = inode;
    return this;
}

void dir_entry_delete(dir_entry_t *this)
{
    assert(this->chain.next == NULL);
    delete(this);
}

void dir_add_entry(inode_t *this, char*name, inode_t *inode)
{
    dir_entry_t *dir_entry = dir_entry_new(name, inode);
    list_push_tail(&this->dir_entry_list, &dir_entry->chain);
}

void dir_remove_entry(inode_t *this, char *name)
{
    chain_t *chain;

    for_each (chain, this->dir_entry_list) {
        dir_entry_t *dir_entry = host_of(dir_entry_t, chain, chain);
        if (strcmp(dir_entry->name, name) == 0) {
            chain_unlink(chain);
            dir_entry_delete(dir_entry);
            return;
        }
    }
}

inode_t *dir_new()
{
    inode_t *this = new(inode_t);

    this->magic = INODE_MAGIC;
    this->type = INODE_DIR;
    this->size = 0;
    list_init(&this->dir_entry_list);
    dir_add_entry(this, ".", this);
    dir_add_entry(this, "..", this);
    return this;
}

//string operations
static int split_string(char *string, char *seperator, char *word_table[])
{
    char *word;
    int word_count = 0;

    word = strtok(string, seperator);
    while (word) {
        word_table[word_count++] = word;
        strtok(NULL, seperator);
    }
    return word_count;
}

inode_t *resolv_path(int flag, const char *path_read_only, char *base_name)
{
    char path [MAX_PATH_NAME];
    char *word_table[MAX_PATH_COMPONENTS];
    int word_count;
    int i;
    inode_t *inode;

    strcpy(path, path_read_only);
    assert(path[0] == '/');
    inode = root_dir;
    word_count = split_string(path, "/", word_table);
    
    for (i = 0; i < word_count; i++) {
        char *word = word_table[i];

        if (i == word_count - 1 && flag == RESOLV_PART) {
           strcpy(base_name, word);
           return inode;
        }

        inode = dir_lookup(inode, word);
        if (!inode)
            return NULL;

        if (i != word_count - 1 && inode->type != INODE_DIR )
            return inode;
    }
    return inode;
}

//the path of directory
inode_t *resolv_full_path(const char *path) 
{
    return resolv_path(RESOLV_FULL, path, NULL);
}

//the path of file
inode_t *resolv_part_path(const char *path, char *base_name)
{
        return resolv_path(RESOLV_PART, path, base_name);
}

