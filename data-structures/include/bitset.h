#ifndef BITSET_H
#define BITSET_H
#include "graph.h"
#include <stddef.h>
#include <stdint.h>

// Return status
#define MAX_LIMIT_REACHED 1
#define NON_PRIMITIVE_SET 2

typedef struct {
    uint64_t *nodes;
    size_t n_blocks;
    unsigned int weight_sum;
} bitset_t;

bitset_t *bitset_new(size_t universe_length);
bitset_t *bitset_new_from(size_t universe_length, bitset_t *src);
int bitset_insert(bitset_t *set, node_t *node, unsigned int limit);
bool bitsets_are_disjoint(bitset_t *s1, bitset_t *s2);
void bitset_free(bitset_t **set);

#endif
