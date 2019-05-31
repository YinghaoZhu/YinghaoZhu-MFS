#include "root.h"
#include "block.h"
#include "log.h"


int block_size;
int entries_per_block;
int total_block_number;
int free_block_number;
list_t free_block_list;//available block list

//number of block be used 
int block_number_of(int size)
{
    return (size + block_size - 1) / block_size;//exclude size = 0;
}

/* Assume total_block_number and block_size has been initialized */
void blocks_init()
{
    int i;

    entries_per_block = block_size / sizeof(void *);
    free_block_number = 0;
    
    list_init(&free_block_list);
    for (i = 0; i < total_block_number; i++) {
        block_t *block = (block_t *)malloc(block_size);

        if (!block)
            log_panic("blocks_init: malloc failed!\n");
        free_block(block);
    }
    printf("block_size = %d\n", block_size);
    printf("total_block_number = %d\n", total_block_number);
    printf("max_file_size = %d\n", entries_per_block * block_size);
}

void *alloc_block()
{
    chain_t *chain = list_pop_head(&free_block_list);
    if (!chain) {
        log_error("alloc_block() failed!\n");
        return NULL;
    }

    --free_block_number;
    return host_of(block_t, chain, chain);
}

void free_block(void *pointer)
{
    block_t *block = (block_t *)pointer;
    chain_init(&block->chain);
    list_push_tail(&free_block_list, &block->chain);
    ++free_block_number;
}
