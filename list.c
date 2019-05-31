#include"root.h"
#include"list.h"

//initialize the list 
void list_init(list_t *this)
{
    this->next = this->prev = this;
}

//delete one inode from list
void list_unlink(list_t *this, chain_t *chain)
{
    chain_unlink(chain);
}

//push a head into the list 
void list_push_head(list_t *this, chain_t *chain)
{
    assert(chain->next == NULL);
    chain_link3(this, chain, this->next);
}

/*get head fron the list
 * new head become the next inode
 * of head
 */
chain_t *list_pop_head(list_t *this)
{
    chain_t *head;
    if (list_is_empty(this))
        return NULL;
    head = this->next;
    chain_unlink(head);
    return head;
}

//insert one after the end of list
void list_push_tail(list_t *this, chain_t *chain)
{
    assert(chain->next == NULL);
    chain_link3(this->prev, chain, this);
}

//release and return the tail of list
chain_t *list_pop_tail(list_t *this)
{
    chain_t *tail;

    if (list_is_empty(this))
        return NULL;
    tail = this->prev;
    chain_unlink(tail);
    return tail;
}
