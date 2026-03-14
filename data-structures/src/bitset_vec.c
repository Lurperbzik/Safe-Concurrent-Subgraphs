#include "bitset_vec.h"
#include <stdlib.h>

bitset_vec_t *bitset_vec_new(void) {
    bitset_vec_t *vector = malloc(sizeof(bitset_vec_t));
    if (vector == NULL)
        return NULL;

    bitset_t **bitsets = calloc(INITIAL_CAPACITY, sizeof(bitset_t *));
    if (bitsets == NULL) {
        free(vector);
        return NULL;
    }

    vector->size = 0;
    vector->capacity = INITIAL_CAPACITY;
    vector->bitsets = bitsets;
    return vector;
}

int bitset_vec_push(bitset_vec_t *vect, bitset_t *bitset) {
    if (vect->size == vect->capacity) {
        size_t new_capacity = vect->capacity * 2;
        bitset_t **new =
            realloc(vect->bitsets, new_capacity * sizeof(bitset_t *));
        if (new == NULL)
            return -1;
        vect->bitsets = new;
        vect->capacity = new_capacity;
    }
    vect->bitsets[vect->size++] = bitset;
    return 0;
}

void bitset_vec_pop(bitset_vec_t *vect) {
    bitset_free(&vect->bitsets[--vect->size]);
}