#include "graph.h"
#include <stddef.h>
#include <stdio.h>

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

#define DIM 12

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
        return 1;
    }
    for (int i = 0; i < DIM; i++) {
        printf("\nnode_t %d :", i);
        printf("\t{");
        printf("\tid: %d", graph.nodes[i].id);
        printf("\tweight: %d", graph.nodes[i].weight);
        printf("\tin_degree: %d", graph.nodes[i].in_degree);
        printf("\thas_mult_outs: %d", graph.nodes[i].has_mult_outs);
        printf("\t}");
    }
    printf("\n\nPATH node_t A: \n");
    node_t *a = graph.nodes;
    print_path(graph.nodes, a, true);
    return 0;
}
