#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    unsigned int dim;
    int *nodes_weights;
    bool *edges;
} adj_matrix_t;

typedef struct node_t {
    unsigned int id;
    unsigned int weight;
    int in_degree;
    struct node_t **inputs;
    bool has_mult_outs;
} node_t;

typedef struct {
    node_t *nodes;
    size_t size;
} graph_t;

graph_t create_from_adj_matrix(adj_matrix_t *matrix);
#endif
