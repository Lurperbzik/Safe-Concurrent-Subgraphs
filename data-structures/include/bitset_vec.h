#ifndef BITSET_VEC_H
#define BITSET_VEC_H
#include "bitset.h"
#include "graph.h"

#define INITIAL_CAPACITY 4

typedef struct {
    bitset_t **bitsets;
    size_t size;
    size_t capacity;
} bitset_vec_t;

bitset_vec_t *bitset_vec_new(void);
int bitset_vec_push(bitset_vec_t *vect, bitset_t *bitset);
void bitset_vec_pop(bitset_vec_t *vect);
#endif