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
    size_t in_degree;
    size_t out_degree;
    struct node_t **inputs;
    struct node_t **outputs;
} node_t;

typedef struct {
    node_t *nodes;
    struct node_t **edges_in;
    struct node_t **edges_out;
    size_t size;
} graph_t;
graph_t create_from_adj_matrix(adj_matrix_t *matrix);
#endif
