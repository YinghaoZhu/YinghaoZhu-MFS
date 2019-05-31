#ifndef _ROOT_H
#define _ROOT_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include "log.h"

static inline long min(long x, long y)
{
    return x < y ? x : y;
}

static inline long max(long x, long y)
{
    return x > y ? x : y;
}

static inline int round_up(int size, int align)
{
    return (size + align -1) / align *align;
}

static inline int roung_down(int size, int align)
{
    return size / align * align;
}

#define new(type) ((type *) malloc(sizeof(type)))
#define delete(pointer) free(pointer)

#endif
