#include "bitset.h"
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 64

bitset_t *bitset_new(size_t universe_length) {
    size_t n_blocks = universe_length / BLOCK_SIZE;
    if (universe_length % BLOCK_SIZE != 0)
        n_blocks++;

    uint64_t *blocks = calloc(n_blocks, BLOCK_SIZE);
    if (blocks == NULL)
        return NULL;

    bitset_t *new = malloc(sizeof(bitset_t));
    if (new == NULL) {
        free(blocks);
        return NULL;
    };
    new->n_blocks = n_blocks;
    new->nodes = blocks;
    new->weight_sum = 0;
    new->n_elements = 0;
    return new;
}

bitset_t *bitset_new_from(size_t universe_length, bitset_t *src) {
    bitset_t *new = bitset_new(universe_length);
    if (new == NULL)
        return NULL;
    memcpy(new->nodes, src->nodes, BLOCK_SIZE * src->n_blocks);
    new->weight_sum = src->weight_sum;
    new->n_elements = src->n_elements;
    new->n_blocks = src->n_blocks;
    return new;
}

int bitset_insert(bitset_t *set, node_t *node, unsigned int limit) {
    if (set->weight_sum >= limit)
        return MAX_LIMIT_REACHED;

    size_t index = node->id / BLOCK_SIZE;
    size_t offset = node->id % BLOCK_SIZE;
    set->nodes[index] |= (1ULL << offset);
    set->weight_sum += node->weight;
    set->n_elements++;

    if (node->has_mult_outs) {
        if (set->weight_sum >= limit)
            return MAX_LIMIT_REACHED;
        if (set->weight_sum != node->weight)
            return NON_PRIMITIVE_SET;
    }
    return 0;
}

void bitset_insert_2(bitset_t *set, size_t id) {
    size_t index = id / BLOCK_SIZE;
    size_t offset = id % BLOCK_SIZE;
    set->nodes[index] |= (1ULL << offset);
    set->n_elements++;
}

bool bitsets_are_disjoint(bitset_t *s1, bitset_t *s2) {
    for (size_t i = 0; i < s1->n_blocks; i++) {
        if (s1->nodes[i] & s2->nodes[i])
            return false;
    }
    return true;
}

void bitset_free(bitset_t **set) {
    if (set == NULL || *set == NULL)
        return;
    free((*set)->nodes);
    (*set)->nodes = NULL;
    free(*set);
    *set = NULL;
}

void bitsets_disjoint_union(bitset_t *dest, bitset_t *src) {
    for (size_t i = 0; i < dest->n_blocks; i++) {
        dest->nodes[i] |= src->nodes[i];
    }
    dest->n_elements += src->n_elements;
}

void bitsets_union(bitset_t *dest, bitset_t *src) {
    dest->n_elements = 0;
    for (size_t i = 0; i < dest->n_blocks; i++) {
        dest->nodes[i] |= src->nodes[i];
        // instruction for 64 bits block size
        dest->n_elements += __builtin_popcountll(dest->nodes[i]);
    }
}

bool bitset_contains(bitset_t *bset, size_t index) {
    return (bset->nodes[index / BLOCK_SIZE] >> (index % BLOCK_SIZE)) & 1;
}