#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    unsigned int dim;
    int *nodes_weights;
    bool *edges;
} AdjMatrix;

typedef struct Node {
    unsigned int id;
    unsigned int weight;
    int in_degree;
    struct Node **inputs;
    bool has_mult_outs;
} Node;

typedef struct {
    Node *nodes;
    size_t size;
} Graph;

Graph create_from_adj_matrix(AdjMatrix matrix);
#endif
