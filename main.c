#include "bitset.h"
#include "bitset_vec.h"
#include "graph.h"
#include <stdio.h>

#define DIM 12
#define LIMIT 5

int safe_concurrent_subgraphs(graph_t graph, bitset_vec_t *vect);
int exhaustive_search_sets(graph_t graph, bitset_vec_t *vect);
int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                      size_t universe_length);
bitset_vec_t *max_disjoint_sets(bitset_vec_t *vect, size_t universe_length);
void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
               bitset_vec_t *selected, bitset_vec_t **best,
               size_t universe_length, size_t total_sets);

void print_binary(uint64_t n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        printf("%ld", (n >> i) & 1);
}

int main(void) {
    // clang-format off
    bool edges[DIM * DIM] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    // clang-format on
    int nodes_weights[DIM] = {1, 1, 1, 1, 1, 3, 2, 3, 6, 2, 1, 4};
    adj_matrix_t adj = {
        .dim = DIM, .edges = edges, .nodes_weights = nodes_weights};

    graph_t graph = create_from_adj_matrix(&adj);
    if (graph.nodes == NULL) {
        printf("Error creating graph\n");
        return 1;
    }
    bitset_vec_t *vector = bitset_vec_new();
    if (vector == NULL) {
        printf("Error creating bitset vector\n");
        return 2;
    }
    if (exhaustive_search_sets(graph, vector) != 0) {
        printf("Error obteniendo subgrafos");
    } else {
        printf("Número de subgrafos obtenidos: %ld\n", vector->size);
    }
    bitset_vec_t *solution = max_disjoint_sets(vector, graph.size);

    printf("Tamaño de la solución máxima: %ld\n", solution->size);
    for (size_t i = 0; i < solution->size; i++) {
        printf("Set : ");
        for (size_t j = 0; j < solution->bitsets[0]->n_blocks; j++) {
            print_binary(solution->bitsets[i]->nodes[j]);
            printf(" ");
        }
        printf("\n");
    }
    return 0;
}

int exhaustive_search_sets(graph_t graph, bitset_vec_t *vect) {
    bitset_t *bset;
    for (size_t i = 0; i < graph.size; i++) {
        bset = bitset_new(graph.size);
        if (bset == NULL)
            return -1;

        if (bitset_vec_push(vect, bset) != 0)
            return -1;

        if (deep_search_paths(bset, &graph.nodes[i], vect, graph.size) != 0)
            return -1;
    }
    return 0;
}

int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                      size_t universe_length) {
    bitset_t *new;
    bitset_insert(bset, node);
    if (bset->weight_sum >= LIMIT) {
        return 0;
    }
    if (node->has_mult_outs) {
        bset->weight_sum = 0;
    }

    if (deep_search_paths(bset, node->inputs[0], vect, universe_length) != 0)
        return -1;

    for (int i = 1; i < node->in_degree; i++) {
        new = bitset_new_from(universe_length, bset);
        if (new == NULL)
            return -1;
        if (bitset_vec_push(vect, new) != 0)
            return -1;

        if (deep_search_paths(new, node->inputs[i], vect, universe_length) != 0)
            return -1;
    }
    return 0;
}

bitset_vec_t *max_disjoint_sets(bitset_vec_t *vect, size_t universe_length) {
    size_t index = 0;
    bitset_t *current_union = bitset_new(universe_length);
    bitset_vec_t *selected = bitset_vec_new();
    bitset_vec_t *best = bitset_vec_new();
    backtrack(vect, index, current_union, selected, &best, universe_length, 0);
    return best;
}

void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
               bitset_vec_t *selected, bitset_vec_t **best,
               size_t universe_length, size_t total_sets) {

    if (selected->size + vect->size - index <= (*best)->size)
        return;

    if (index >= vect->size) {
        if (selected->size > (*best)->size && total_sets == universe_length) {
            bitset_vec_free(best);
            bitset_vec_copy(best, selected);
        }
        return;
    }

    // Branch 1
    if (bitsets_are_disjoint(vect->bitsets[index], current_union)) {
        bitset_vec_t *new_selected;
        bitset_vec_copy(&new_selected, selected);
        bitset_vec_push(new_selected, vect->bitsets[index]);
        bitset_t *new_union = bitset_new_from(universe_length, current_union);
        bitsets_union(new_union, vect->bitsets[index]);
        backtrack(vect, index + 1, new_union, new_selected, best,
                  universe_length,
                  total_sets + vect->bitsets[index]->n_elements);
        bitset_vec_free(&new_selected);
        bitset_free(&new_union);
    }
    // Branch 2
    backtrack(vect, index + 1, current_union, selected, best, universe_length,
              total_sets);
}
