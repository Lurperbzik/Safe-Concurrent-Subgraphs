#include "bitset_vec.h"
#include <stdlib.h>

BitSetVector *bitset_vec_new(void) {
    BitSetVector *vector = malloc(sizeof(BitSetVector));
    if (vector == NULL)
        return NULL;

    BitSet **bitsets = calloc(INITIAL_CAPACITY, sizeof(BitSet *));
    if (bitsets == NULL) {
        free(vector);
        return NULL;
    }

    vector->size = 0;
    vector->capacity = INITIAL_CAPACITY;
    vector->bitsets = bitsets;
    return vector;
}

int bitset_vec_push(BitSetVector *vect, BitSet *bitset) {
    if (vect->size == vect->capacity) {
        size_t new_capacity = vect->capacity * 2;
        BitSet **new = realloc(vect->bitsets, new_capacity * sizeof(BitSet *));
        if (new == NULL)
            return -1;
        vect->bitsets = new;
        vect->capacity = new_capacity;
    }
    vect->bitsets[vect->size++] = bitset;
    return 0;
}

void bitset_vec_pop(BitSetVector *vect) {
    bitset_free(&vect->bitsets[--vect->size]);
}