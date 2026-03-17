#include "graph.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * La matriz de adyacencia en este caso tiene un formato específico
 * que consta de una matriz boolena de aristas y un vector de pesos
 * El grafo resultante es una lista adyacente de memoria que almacena
 * los nodos
 * Los punteros de entrada y salida de los nodos también se almacenan en
 * una lista adyacente en memoria
 */

graph_t create_from_adj_matrix(adj_matrix_t *matrix) {
    graph_t graph = {0};

    size_t *in_degree = calloc(matrix->dim, sizeof(size_t));
    size_t *out_degree = calloc(matrix->dim, sizeof(size_t));
    if (in_degree == NULL || out_degree == NULL)
        goto cleanup_degrees;

    size_t n_edges = 0;
    for (size_t i = 0; i < matrix->dim; i++) {
        for (size_t j = 0; j < matrix->dim; j++) {
            if (matrix->edges[i * matrix->dim + j] != 0) {
                out_degree[i]++;
                in_degree[j]++;
                n_edges++;
            }
        }
    }

    graph.nodes = calloc(matrix->dim, sizeof(node_t));
    if (graph.nodes == NULL)
        goto cleanup_degrees;

    graph.edges_in = malloc(n_edges * sizeof(node_t *));
    graph.edges_out = malloc(n_edges * sizeof(node_t *));
    if (graph.edges_in == NULL || graph.edges_out == NULL)
        goto cleanup_all;

    size_t offset_in = 0, offset_out = 0;
    for (unsigned int i = 0; i < matrix->dim; i++) {
        graph.nodes[i].id = i;
        graph.nodes[i].weight = matrix->nodes_weights[i];
        graph.nodes[i].in_degree = in_degree[i];
        graph.nodes[i].out_degree = out_degree[i];
        graph.nodes[i].inputs = &graph.edges_in[offset_in];
        graph.nodes[i].outputs = &graph.edges_out[offset_out];
        offset_in += in_degree[i];
        offset_out += out_degree[i];
    }

    memset(in_degree, 0, matrix->dim * sizeof(size_t));
    memset(out_degree, 0, matrix->dim * sizeof(size_t));

    for (unsigned int i = 0; i < matrix->dim; i++) {
        for (unsigned int j = 0; j < matrix->dim; j++) {
            if (matrix->edges[i * matrix->dim + j] != 0) {
                graph.nodes[j].inputs[in_degree[j]++] = &graph.nodes[i];
                graph.nodes[i].outputs[out_degree[i]++] = &graph.nodes[j];
            }
        }
    }

    graph.size = matrix->dim;
    free(in_degree);
    free(out_degree);
    return graph;

cleanup_all:
    free(graph.nodes);
    free(graph.edges_in);
    free(graph.edges_out);
    graph.nodes = NULL;
    graph.edges_in = NULL;
    graph.edges_out = NULL;
cleanup_degrees:
    free(in_degree);
    free(out_degree);
    return graph;
}