#include "graph.h"
#include <stdbool.h>
#include <stdlib.h>

/*
 * La matriz de adyacencia en este caso tiene un formato específico
 * que consta de una matriz boolena de aristas y un vector de pesos
 * El grafo resultante es una lista adyacente de memoria que almacena
 * los nodos
 * Los punteros de entrada y salida de los nodos también se almacenan en
 * una lista adyacente en memoria
 */

Graph create_from_adj_matrix(AdjMatrix matrix) {
    Graph graph = {0};
    graph.nodes = calloc(matrix.dim, sizeof(Node));
    if (graph.nodes == NULL)
        return graph;

    size_t n_edges = 0;
    int mult_outs;
    for (unsigned int i = 0; i < matrix.dim; i++) {
        mult_outs = 0;
        for (unsigned int j = 0; j < matrix.dim; j++) {
            if (matrix.edges[i * matrix.dim + j] != 0) {
                n_edges++;
                mult_outs++;
            }
        }
        if (mult_outs > 1)
            graph.nodes[i].has_mult_outs = true;
    }
    Node **edges = malloc(n_edges * sizeof(Node *));
    if (edges == NULL) {
        free(graph.nodes);
        graph.nodes = NULL;
        return graph;
    }
    for (unsigned int i = 0; i < matrix.dim; i++) {
        graph.nodes[i].id = i;
        graph.nodes[i].weight = matrix.nodes_weights[i];
    }
    size_t index = 0;
    for (unsigned int i = 0; i < matrix.dim; i++) {
        for (unsigned int j = 0; j < matrix.dim; j++) {
            if (matrix.edges[j * matrix.dim + i] != 0) {
                edges[index] = &graph.nodes[j];
                if (graph.nodes[i].in_degree == 0) {
                    graph.nodes[i].inputs = &edges[index];
                }
                graph.nodes[i].in_degree++;
                index++;
            }
        }
    }
    graph.size = matrix.dim;
    return graph;
}
