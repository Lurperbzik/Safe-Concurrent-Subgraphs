#include "bitset.h"
#include "bitset_vec.h"
#include "graph.h"
#include <stdio.h>

#define DIM 12
#define LIMIT 5

void print_path(node_t *base, node_t *node, bool init);
int safe_concurrent_subgraphs(graph_t graph, bitset_vec_t *vect);
int get_subgraph(bitset_t *bset, node_t node, bitset_vec_t *vect,
                 size_t universe_length);

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

    graph_t graph = create_from_adj_matrix(adj);
    if (graph.nodes == NULL) {
        printf("Error creating graph\n");
        return 1;
    }
    bitset_vec_t *vector = bitset_vec_new();
    if (vector == NULL) {
        printf("Error creating bitset vector\n");
        return 2;
    }
    if (safe_concurrent_subgraphs(graph, vector) != 0) {
        printf("Error obteniendo subgrafos");
    } else {
        printf("Número de subgrafos obtenidos: %ld\n", vector->size);
    }
    return 0;
}

int safe_concurrent_subgraphs(graph_t graph, bitset_vec_t *vect) {
    bitset_t *bset;
    for (size_t i = 0; i < graph.size; i++) {
        bset = bitset_new(graph.size);
        if (bset == NULL)
            return -1;

        if (bitset_vec_push(vect, bset) != 0)
            return -1;

        if (get_subgraph(bset, graph.nodes[i], vect, graph.size) != 0)
            return -1;
    }
    return 0;
}

int get_subgraph(bitset_t *bset, node_t node, bitset_vec_t *vect,
                 size_t universe_length) {
    bitset_t *new;
    int status = bitset_insert(bset, node, LIMIT);
    if (status == MAX_LIMIT_REACHED) {
        return 0;
    }
    if (status == NON_PRIMITIVE_SET) {
        bitset_vec_pop(vect);
        return 0;
    }
    if (get_subgraph(bset, *node.inputs[0], vect, universe_length) != 0)
        return -1;

    for (int i = 1; i < node.in_degree; i++) {
        new = bitset_new_from(universe_length, *bset);
        // Error with memory
        if (new == NULL)
            return -1;
        // Error with memory
        if (bitset_vec_push(vect, new) != 0)
            return -1;

        if (get_subgraph(new, *node.inputs[i], vect, universe_length) != 0)
            return -1;
    }
    return 0;
}

void print_path(node_t *base, node_t *node, bool init) {
    if (node == base && !init) {
        printf("\n");
        return;
    }

    for (int i = 0; i < node->in_degree; i++) {
        printf("%d\t", node->id);
        print_path(base, node->inputs[i], false);
    }
}