#include "graph.h"
#include <stddef.h>
#include <stdio.h>

void print_input_path(node_t *base, node_t *node, bool init) {
    if (node == base && !init) {
        printf("\n");
        return;
    }

    for (size_t i = 0; i < node->in_degree; i++) {
        printf("%d\t", node->id);
        print_input_path(base, node->inputs[i], false);
    }
}
void print_output_path(node_t *base, node_t *node, bool init) {
    if (node == base && !init) {
        printf("\n");
        return;
    }

    for (size_t i = 0; i < node->out_degree; i++) {
        printf("%d\t", node->id);
        print_output_path(base, node->outputs[i], false);
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
        printf("\tin_degree: %ld", graph.nodes[i].in_degree);
        printf("\tout_degree: %ld", graph.nodes[i].out_degree);
        printf("\t}");
    }
    node_t *a = graph.nodes;
    printf("\n\nPATH inputs A: \n");
    print_input_path(graph.nodes, a, true);
    printf("\n\nPATH outputs A: \n");
    print_output_path(graph.nodes, a, true);
    return 0;
}
