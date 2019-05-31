#ifndef _UTILS_LIST_H
#define _UTILS_LIST_H

#define host_of(type, member, pointer)                                       \
    ((type*) ((ulong)pointer - offsetof(type, member)))

        
#define for_each(chain, list)           \
        for (chain = (list).next;       \
            chain != &list;             \
            chain = chain->next)

#define for_each_reverse(chain, list)   \
        for (chain = (list).prev;       \
            chain != &list;             \
            chain = chain->prev )

#define for_each_safe(chain, next_chain, list)                \
        for (chain = list.next, next_chain = chain->m=next;   \
             chain != &list;                                  \
             chain = next_chain, next_chain = next_chain->next) 
#include<stddef.h>
struct chain;

typedef struct chain chain_t;
typedef chain_t list_t;

struct chain {
     chain_t *next, *prev;
};

//initialize the chain 
static inline void chain_init(chain_t *this)
{
    this->next = NULL;
    this->prev = NULL;
}

//link list
static inline void chain_link(chain_t *left, chain_t *right)
{
    left->next = right;
    right->prev = left;
}

// insert inode chain between left and right
static inline void chain_link3(chain_t *left, chain_t *chain, chain_t *right)
{
    chain_link(left, chain);
    chain_link(chain, right);
}

//delete inode "this" from chain 
static inline void chain_unlink(chain_t *this)
{
    chain_link(this->prev, this->next);
    this->prev = NULL;
    this->next = NULL;
}
//insert one inode before "this"
static inline void chain_insert_before(chain_t *this, chain_t *new_chain)
{
    chain_link3(this->prev, new_chain, this);
}
//insert inode after "this"
static inline void chain_insert_after(chain_t *this, chain_t *new_chain)
{ 
    chain_link3(this, new_chain, this->next);
}

#define list_declare(list) list_t list = {&list, &list}

extern void list_init(list_t *this);
extern void list_push_head(list_t *this, chain_t *chain);
extern chain_t *list_pop_head(list_t *this);
extern void list_push_tail(list_t *this, chain_t *chain);
extern chain_t *list_pop_tail(list_t *this);

static inline int list_is_empty(list_t *this)
{
    return this->next == this;
}

//return the head of list
static inline chain_t *list_head(list_t *this)
{
    return list_is_empty(this) ? NULL : this->next;
}

//return the tail of list 
static inline chain_t *list_tail(list_t *this)
{
    return list_is_empty(this) ? NULL : this->prev;
}

#endif
