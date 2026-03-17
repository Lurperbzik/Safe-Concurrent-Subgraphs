
#include "algorithm.h"
#include <stdio.h>

#define MAX_LIMIT_REACHED 0
#define NON_PRIMITIVE_SET 0

bitset_vec_t *max_disjoint_sets(bitset_vec_t *vect, adj_matrix_t *matrix);
static bitset_vec_t *exhaustive_search_sets(graph_t *graph, unsigned int limit);
static int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                             size_t universe_length, unsigned int limit);
static void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
                      bitset_vec_t **selected, bitset_vec_t **best,
                      adj_matrix_t *matrix, graph_t *graph, unsigned int limit);
static void graph_reconstruct(bitset_t *final_union, bitset_vec_t **selected,
                              bitset_vec_t **best, adj_matrix_t *matrix,
                              graph_t *graph, unsigned int limit);
static bool join_entry(size_t index, bitset_vec_t *selected, graph_t *graph,
                       unsigned int limit);
static bool join_exit(size_t index, bitset_vec_t *selected, graph_t *graph,
                      unsigned int limit);
static void join_both(size_t index, bitset_vec_t **selected,
                      adj_matrix_t *matrix);

bitset_vec_t *safe_concurrent_subgraphs(graph_t *graph, adj_matrix_t *matrix,
                                        unsigned int limit) {
    // Get all primitive sets
    bitset_vec_t *vect = exhaustive_search_sets(graph, limit);
    fprintf(stdout, "Número de primitive sets: %ld\n", vect->size);
    // get maximum disjoint sets
    size_t index = 0;
    bitset_t *current_union = bitset_new(matrix->dim);
    bitset_vec_t *selected = bitset_vec_new();
    bitset_vec_t *best = bitset_vec_new();
    backtrack(vect, index, current_union, &selected, &best, matrix, graph,
              limit);
    fprintf(stdout, "Tamaño de la solución: %ld\n", best->size);
    return best;
}

static bitset_vec_t *exhaustive_search_sets(graph_t *graph,
                                            unsigned int limit) {
    bitset_vec_t *vect = bitset_vec_new();
    bitset_t *bset;
    for (size_t i = 0; i < graph->size; i++) {
        bset = bitset_new(graph->size);
        if (bset == NULL)
            return NULL;

        if (bitset_vec_push(vect, bset) != 0)
            return NULL;

        if (deep_search_paths(bset, &graph->nodes[i], vect, graph->size,
                              limit) != 0)
            return NULL;
    }
    return vect;
}

static int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                             size_t universe_length, unsigned int limit) {
    bitset_t *new;
    if (bset->weight_sum >= limit)
        return MAX_LIMIT_REACHED;
    bitset_insert(bset, node->id, node->weight);

    if (node->out_degree > 1) {
        if (bset->weight_sum >= limit)
            return MAX_LIMIT_REACHED;
        if (bset->weight_sum != node->weight) {
            new = bitset_vec_pop(vect);
            bitset_free(&new);
            return NON_PRIMITIVE_SET;
        }
    }

    if (deep_search_paths(bset, node->inputs[0], vect, universe_length,
                          limit) != 0)
        return -1;

    for (size_t i = 1; i < node->in_degree; i++) {
        new = bitset_new_from(universe_length, bset);
        if (new == NULL)
            return -1;
        if (bitset_vec_push(vect, new) != 0)
            return -1;

        if (deep_search_paths(new, node->inputs[i], vect, universe_length,
                              limit) != 0)
            return -1;
    }
    return 0;
}

static void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
                      bitset_vec_t **selected, bitset_vec_t **best,
                      adj_matrix_t *matrix, graph_t *graph,
                      unsigned int limit) {

    if ((*selected)->size + vect->size - index <= (*best)->size)
        return;

    if (index >= vect->size) {
        if ((*selected)->size > (*best)->size) {
            // bitset_vec_free(best);
            // bitset_vec_copy(best, *selected);

            graph_reconstruct(current_union, selected, best, matrix, graph,
                              limit);
        }
        return;
    }

    // Branch 1
    if (bitsets_are_disjoint(vect->bitsets[index], current_union)) {
        bitset_vec_t *new_selected;
        bitset_vec_copy(&new_selected, *selected);
        bitset_vec_push(new_selected, vect->bitsets[index]);
        bitset_t *new_union = bitset_new_from(matrix->dim, current_union);
        bitsets_disjoint_union(new_union, vect->bitsets[index]);
        backtrack(vect, index + 1, new_union, &new_selected, best, matrix,
                  graph, limit);
        bitset_vec_free(&new_selected);
        bitset_free(&new_union);
    }
    // Branch 2
    backtrack(vect, index + 1, current_union, selected, best, matrix, graph,
              limit);
}

static void graph_reconstruct(bitset_t *final_union, bitset_vec_t **selected,
                              bitset_vec_t **best, adj_matrix_t *matrix,
                              graph_t *graph, unsigned int limit) {

    if (final_union->n_elements == matrix->dim) {
        bitset_vec_free(best);
        bitset_vec_copy(best, *selected);
        return;
    }
    for (size_t i = 0; i < final_union->n_blocks; i++) {
        uint64_t block = final_union->nodes[i];
        uint64_t mask = (i == final_union->n_blocks - 1)
                            ? (1ULL << (matrix->dim % 64)) - 1
                            : ~0ULL;
        uint64_t zeros = ~block & mask;
        // Make reconstruction graph
        while (zeros) {
            size_t pos = __builtin_ctzll(zeros);
            if (join_exit(pos, *selected, graph, limit))
                goto next_i;
            if (join_entry(pos, *selected, graph, limit))
                goto next_i;
            join_both(pos, selected, matrix);

        next_i:
            zeros &= zeros - 1;
        }
    }

    if ((*selected)->size > (*best)->size) {
        bitset_vec_free(best);
        bitset_vec_copy(best, *selected);
    }
}

static bool check_inputs(node_t *node, bitset_t *bset, unsigned int weight,
                         unsigned int limit) {
    if (weight >= limit)
        return true;
    if (!bitset_contains(bset, node->id))
        return false;
    for (size_t i = 0; i < node->in_degree; i++) {
        node_t *prev = node->inputs[i];
        if (!check_inputs(prev, bset, weight + node->weight, limit))
            return false;
    }
    return true;
}

static bool check_outputs(node_t *node, bitset_t *bset, unsigned int weight,
                          unsigned int limit) {
    if (weight >= limit)
        return true;
    if (!bitset_contains(bset, node->id))
        return false;
    for (size_t i = 0; i < node->out_degree; i++) {
        node_t *next = node->outputs[i];
        if (!check_inputs(next, bset, weight + node->weight, limit))
            return false;
    }
    return true;
}

static bool join_exit(size_t index, bitset_vec_t *selected, graph_t *graph,
                      unsigned int limit) {
    int prev_index = -1;
    node_t node_base = graph->nodes[index];
    for (size_t i = 0; i < node_base.in_degree; i++) {
        prev_index = bitset_vec_get_index(selected, node_base.inputs[i]->id);
        if (prev_index == -1)
            continue;

        if (!check_inputs(node_base.inputs[i], selected->bitsets[prev_index],
                          node_base.weight, limit))
            return false;
    }
    // join the last entry candidate
    // TODO : ¿Backtrack?
    if (prev_index == -1) // TODO: Control this case
        return false;
    bitset_insert(selected->bitsets[prev_index], index, node_base.weight);
    fprintf(stdout, "Joined as exit : %ld\n", index);
    return true;
}

static bool join_entry(size_t index, bitset_vec_t *selected, graph_t *graph,
                       unsigned int limit) {
    // Potential problem with single cyclic nodes : Maeby it's imposible
    int next_index = -1;
    node_t node_base = graph->nodes[index];
    // check inputs
    for (size_t i = 0; i < node_base.in_degree; i++) {
        int prev_index =
            bitset_vec_get_index(selected, node_base.inputs[i]->id);
        if (prev_index == -1)
            continue;

        if (!check_inputs(node_base.inputs[i], selected->bitsets[prev_index], 0,
                          limit))
            return false;
    }
    // check outputs
    for (size_t i = 0; i < node_base.out_degree; i++) {
        next_index = bitset_vec_get_index(selected, node_base.outputs[i]->id);
        if (next_index == -1)
            continue;

        if (!check_outputs(node_base.inputs[i], selected->bitsets[next_index],
                           node_base.weight, limit))
            return false;
    }
    // join the last exit candidate
    // TODO : ¿Backtrack?
    if (next_index == -1) // TODO: Control this case
        return false;
    bitset_insert(selected->bitsets[next_index], index, node_base.weight);
    fprintf(stdout, "Joined as entry : %ld\n", index);
    return true;
}

static void join_both(size_t index, bitset_vec_t **selected,
                      adj_matrix_t *matrix) {
    /*
     *  join(element, previous[0], next[0])
     */
    size_t index_next;
    size_t index_prev;
    int status_next = -1;
    int status_prev = -1;
    bitset_vec_t *new = bitset_vec_new();
    // get next[0]
    for (size_t i = 0; i < matrix->dim; i++) {
        if (index != i && matrix->edges[index * matrix->dim + i]) {
            status_next = bitset_vec_get_index(*selected, i);

            if (status_next == -1) {
                continue;
            } else {
                index_next = (size_t)status_next;
                break;
            }
        }
    }
    for (size_t i = 0; i < matrix->dim; i++) {
        // get previous
        if (index != i && matrix->edges[index + matrix->dim * i]) {
            status_prev = bitset_vec_get_index(*selected, i);

            if (status_prev == -1) {
                continue;
            } else {
                index_prev = (size_t)status_prev;
                break;
            }
        }
    }
    /*
     * TODO: Comprobar este caso
     */
    if (status_next == -1 || status_prev == -1) {
        fprintf(stdout, "Next: %d | Prev: %d\n", status_next, status_prev);
    }
    bitsets_union((*selected)->bitsets[index_next],
                  (*selected)->bitsets[index_prev]);
    bitset_insert((*selected)->bitsets[index_next], index, 0);
    for (size_t i = 0; i < (*selected)->size; i++) {
        if (i != index_prev) {
            bitset_vec_push(new, (*selected)->bitsets[i]);
        }
    }
    fprintf(stdout, "Merged by :%ld\n", index);
    bitset_vec_free(selected);
    bitset_vec_copy(selected, new);
}