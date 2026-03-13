#include "bit_set.h"
#include <stdlib.h>

PointerSet *set_new(int universe_legth) {
    size_t n_blocks = universe_legth / BLOCK_SIZE;
    if (universe_legth % BLOCK_SIZE != 0)
        n_blocks++;

    uint64_t *blocks = calloc(n_blocks, BLOCK_SIZE);
    if (blocks == NULL)
        return NULL;

    PointerSet *new = malloc(sizeof(PointerSet));
    if (new == NULL) {
        free(blocks);
        return NULL;
    };
    new->n_blocks = n_blocks;
    new->nodes = blocks;
    // new->weight_sum = 0;
    return new;
}

void set_insert(PointerSet *set, Node node) {
    size_t index = node.id / BLOCK_SIZE;
    size_t offset = node.id % BLOCK_SIZE;
    set->nodes[index] |= (1ULL << offset);
    // Revisar si es a 0 - Eliminar este set directamente
    /*
        if (node.has_mult_outs)
            set->weight_sum = node.weight;
        else
            set->weight_sum += node.weight;
    */
}

bool sets_are_disjoint(PointerSet s1, PointerSet s2) {
    for (size_t i = 0; i < s1.n_blocks; i++) {
        if (s1.nodes[i] & s2.nodes[i])
            return false;
    }
    return true;
}

void free_set(PointerSet **set) {
    if (set == NULL || *set == NULL)
        return;
    free((*set)->nodes);
    (*set)->nodes = NULL;
    free(*set);
    *set = NULL;
}