#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "bitset_vec.h"
#include "graph.h"

bitset_vec_t *safe_concurrent_subgraphs(graph_t *graph, adj_matrix_t *matrix,
                                        unsigned int limit);
#endif