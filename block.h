#ifndef _BLOCK_H
#define _BLOCK_H

#include "list.h"

typedef struct {
    chain_t chain;
    char data[0];
} block_t;

extern int block_size;
extern int entries_per_block;
extern int block_number_of(int size);

extern int total_block_number;
extern int free_block_number;
extern void blocks_init();

extern void *alloc_block();
extern void free_block(void *pointer);

#endif
