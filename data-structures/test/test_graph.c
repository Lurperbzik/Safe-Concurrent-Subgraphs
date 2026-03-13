#include "graph.h"
#include <stddef.h>
#include <stdio.h>

void print_path(Graph graph, Node *node, bool init) {
    if (node == graph && !init) {
        printf("\n");
        return;
    }

    for (int i = 0; i < node->in_degree; i++) {
        printf("%d\t", node->id);
        print_path(graph, node->inputs[i], false);
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
    AdjMatrix adj = {
        .dim = DIM, .edges = edges, .nodes_weights = nodes_weights};

    Graph graph = create_from_adj_matrix(adj);
    if (graph == NULL) {
        return 1;
    }
    for (int i = 0; i < DIM; i++) {
        printf("\nNode %d :", i);
        printf("\t{");
        printf("\tid: %d", graph[i].id);
        printf("\tweight: %d", graph[i].weight);
        printf("\tin_degree: %d", graph[i].in_degree);
        printf("\thas_mult_outs: %d", graph[i].has_mult_outs);
        printf("\t}");
    }
    printf("\n\nPATH Node A: \n");
    Node *a = graph;
    print_path(graph, a, true);
    return 0;
}
