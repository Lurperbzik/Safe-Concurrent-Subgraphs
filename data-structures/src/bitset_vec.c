#include "bitset_vec.h"
#include <stdlib.h>
#include <string.h>

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

bitset_t *bitset_vec_pop(bitset_vec_t *vect) {
    return vect->bitsets[--vect->size];
}

void bitset_vec_free(bitset_vec_t **vect) {
    if (vect == NULL || *vect == NULL)
        return;
    free((*vect)->bitsets); // libera el array interno
    free(*vect);
    *vect = NULL;
}

int bitset_vec_copy(bitset_vec_t **dest, bitset_vec_t *src) {
    *dest = malloc(sizeof(bitset_vec_t));
    if (*dest == NULL)
        return 1;

    memcpy(*dest, src, sizeof(bitset_vec_t));

    (*dest)->bitsets = malloc(src->capacity * sizeof(bitset_t *));
    if ((*dest)->bitsets == NULL) {
        free(*dest);
        return 1;
    }

    memcpy((*dest)->bitsets, src->bitsets, src->size * sizeof(bitset_t *));
    return 0;
}

int bitset_vec_get_index(bitset_vec_t *vect, size_t index) {
    for (size_t i = 0; i < vect->size; i++) {
        if (bitset_contains(vect->bitsets[i], index))
            return i;
    }
    return -1;
}