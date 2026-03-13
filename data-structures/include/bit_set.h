#ifndef BIT_SET_H
#define BIT_SET_H
#include "graph.h"
#include <stddef.h>
#include <stdint.h>

#define REALLOC_ERROR 1
#define BLOCK_SIZE 64
typedef struct {
    uint64_t *nodes;
    size_t n_blocks;
    // int weight_sum;
} PointerSet;

PointerSet *set_new(int universe_length);
void set_insert(PointerSet *set, Node node);
bool sets_are_disjoint(PointerSet s1, PointerSet s2);
void free_set(PointerSet **set);
#endif
