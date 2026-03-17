#include "algorithm.h"
#include <stdio.h>

#define DIM 12
#define LIMIT 5

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
    bitset_vec_t *solution = safe_concurrent_subgraphs(&graph, &adj, LIMIT);

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