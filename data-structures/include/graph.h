#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
typedef struct {
    int dim;
    int *nodes_weights;
    bool *edges;
} AdjMatrix;

typedef struct Node {
    int id;
    int weight;
    int in_degree;
    struct Node **inputs;
    bool has_mult_outs;
} Node;

typedef Node *Graph;

Graph create_from_adj_matrix(AdjMatrix matrix);
#endif
