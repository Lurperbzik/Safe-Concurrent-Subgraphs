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
} BitSet;

BitSet *bitset_new(size_t universe_length);
BitSet *bitset_new_from(size_t universe_length, BitSet src);
int bitset_insert(BitSet *set, Node node, unsigned int limit);
bool bitsets_are_disjoint(BitSet s1, BitSet s2);
void bitset_free(BitSet **set);

#endif
