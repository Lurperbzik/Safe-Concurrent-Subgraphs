#ifndef BITSET_H
#define BITSET_H
#include "graph.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t *nodes;
    size_t n_blocks;
    unsigned int weight_sum;
    size_t n_elements;
} bitset_t;

bitset_t *bitset_new(size_t universe_length);
bitset_t *bitset_new_from(size_t universe_length, bitset_t *src);
void bitset_insert(bitset_t *set, size_t id, unsigned int weight);
bool bitset_contains(bitset_t *bset, size_t index);
bool bitsets_are_disjoint(bitset_t *s1, bitset_t *s2);
void bitsets_disjoint_union(bitset_t *dest, bitset_t *src);
void bitsets_union(bitset_t *dest, bitset_t *src);
void bitset_free(bitset_t **set);
#endif
