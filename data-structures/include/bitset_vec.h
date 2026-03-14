#ifndef BITSET_VEC_H
#define BITSET_VEC_H
#include "bitset.h"
#include "graph.h"

#define INITIAL_CAPACITY 4

typedef struct {
    BitSet **bitsets;
    size_t size;
    size_t capacity;
} BitSetVector;

BitSetVector *bitset_vec_new(void);
int bitset_vec_push(BitSetVector *vect, BitSet *bitset);
void bitset_vec_pop(BitSetVector *vect);
#endif