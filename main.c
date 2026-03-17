#include "bitset.h"
#include "bitset_vec.h"
#include "graph.h"
#include <stdio.h>

#define DIM 12
#define LIMIT 5

int safe_concurrent_subgraphs(graph_t graph, bitset_vec_t *vect);
int exhaustive_search_sets(graph_t graph, bitset_vec_t *vect);
int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                      size_t universe_length);

bitset_vec_t *max_disjoint_sets(bitset_vec_t *vect, adj_matrix_t *matrix);

void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
               bitset_vec_t **selected, bitset_vec_t **best,
               adj_matrix_t *matrix);
void graph_reconstruct(bitset_t *final_union, bitset_vec_t **selected,
                       bitset_vec_t **best, adj_matrix_t *matrix);

bool join_entry(size_t index, bitset_vec_t *selected, adj_matrix_t *matrix);
bool join_exit(size_t index, bitset_vec_t *selected, adj_matrix_t *matrix);
void join_both(size_t index, bitset_vec_t **selected, adj_matrix_t *matrix);
void print_binary(uint64_t n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        printf("%ld", (n >> i) & 1);
}

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
        printf("Error creating graph\n");
        return 1;
    }
    bitset_vec_t *vector = bitset_vec_new();
    if (vector == NULL) {
        printf("Error creating bitset vector\n");
        return 2;
    }
    if (exhaustive_search_sets(graph, vector) != 0) {
        printf("Error obteniendo subgrafos");
    } else {
        printf("Número de subgrafos obtenidos: %ld\n", vector->size);
    }
    bitset_vec_t *solution = max_disjoint_sets(vector, &adj);

    printf("Tamaño de la solución máxima: %ld\n", solution->size);
    for (size_t i = 0; i < solution->size; i++) {
        printf("Set : ");
        for (size_t j = 0; j < solution->bitsets[0]->n_blocks; j++) {
            print_binary(solution->bitsets[i]->nodes[j]);
            printf(" ");
        }
        printf("\n");
    }
    return 0;
}

int exhaustive_search_sets(graph_t graph, bitset_vec_t *vect) {
    bitset_t *bset;
    for (size_t i = 0; i < graph.size; i++) {
        bset = bitset_new(graph.size);
        if (bset == NULL)
            return -1;

        if (bitset_vec_push(vect, bset) != 0)
            return -1;

        if (deep_search_paths(bset, &graph.nodes[i], vect, graph.size) != 0)
            return -1;
    }
    return 0;
}

int deep_search_paths(bitset_t *bset, node_t *node, bitset_vec_t *vect,
                      size_t universe_length) {
    bitset_t *new;
    int status = bitset_insert(bset, node, LIMIT);
    if (status == MAX_LIMIT_REACHED) {
        return 0;
    }
    if (status == NON_PRIMITIVE_SET) {
        new = bitset_vec_pop(vect);
        bitset_free(&new);
        return 0;
    }
    if (deep_search_paths(bset, node->inputs[0], vect, universe_length) != 0)
        return -1;

    for (int i = 1; i < node->in_degree; i++) {
        new = bitset_new_from(universe_length, bset);
        if (new == NULL)
            return -1;
        if (bitset_vec_push(vect, new) != 0)
            return -1;

        if (deep_search_paths(new, node->inputs[i], vect, universe_length) != 0)
            return -1;
    }
    return 0;
}

bitset_vec_t *max_disjoint_sets(bitset_vec_t *vect, adj_matrix_t *matrix) {
    size_t index = 0;
    bitset_t *current_union = bitset_new(matrix->dim);
    bitset_vec_t *selected = bitset_vec_new();
    bitset_vec_t *best = bitset_vec_new();
    backtrack(vect, index, current_union, &selected, &best, matrix);
    return best;
}

void backtrack(bitset_vec_t *vect, size_t index, bitset_t *current_union,
               bitset_vec_t **selected, bitset_vec_t **best,
               adj_matrix_t *matrix) {

    if ((*selected)->size + vect->size - index <= (*best)->size)
        return;

    if (index >= vect->size) {
        if ((*selected)->size > (*best)->size) {
            graph_reconstruct(current_union, selected, best, matrix);
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
        backtrack(vect, index + 1, new_union, &new_selected, best, matrix);
        bitset_vec_free(&new_selected);
        bitset_free(&new_union);
    }
    // Branch 2
    backtrack(vect, index + 1, current_union, selected, best, matrix);
}

void graph_reconstruct(bitset_t *final_union, bitset_vec_t **selected,
                       bitset_vec_t **best, adj_matrix_t *matrix) {

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
            /*
             * New entry of set if the entry set is not exit also
             */
            // if (join_entry(pos, *selected, matrix))
            //     goto next_i;
            /*
             * New exit of set if the exit set is not entry also - TODO: Si es
             * entrada pero entrada + nuevo > L sirve (Vector de pesos!)
             */
            if (join_exit(pos, *selected, matrix))
                goto next_i;
            /*
             * In other case, join sets
             */
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

bool join_entry(size_t index, bitset_vec_t *selected, adj_matrix_t *matrix) {
    /*
     *  for nexts in element:
     *      if !(next is exit of next_set):
     *          join(element, next_set)
     *          return true
     *  return false
     */
    for (size_t i = 0; i < matrix->dim; i++) {
        // get next
        if (index != i && matrix->edges[index * matrix->dim + i]) {
            int bset_index = bitset_vec_get_index(selected, i);
            if (bset_index == -1)
                continue;
            // get all nexts of next and check if all of them are together
            for (size_t j = 0; j < matrix->dim; j++) {
                if (index != j && matrix->edges[i * matrix->dim + j]) {
                    if (!bitset_contains(selected->bitsets[bset_index], j))
                        goto next_i;
                }
            }
            // join
            bitset_insert_2(selected->bitsets[bset_index], index);
            fprintf(stdout, "Joined entry : %ld\n", index);
            return true;
        }
    next_i:;
    }
    return false;
}

bool join_exit(size_t index, bitset_vec_t *selected, adj_matrix_t *matrix) {
    /*
     *  for previous in element:
     *      if !(previous is entry of previous_set):
     *          join(element, previous_set)
     *          return true
     *  return false
     */
    for (size_t i = 0; i < matrix->dim; i++) {
        // get previous
        if (index != i && matrix->edges[index + matrix->dim * i]) {
            int bset_index = bitset_vec_get_index(selected, i);
            if (bset_index == -1)
                continue;
            // get all previous of previous and check if all of them are
            // together
            for (size_t j = 0; j < matrix->dim; j++) {
                if (index != j && matrix->edges[j * matrix->dim + i]) {
                    if (!bitset_contains(selected->bitsets[bset_index], j))
                        goto next_i;
                }
            }
            // join
            bitset_insert_2(selected->bitsets[bset_index], index);
            fprintf(stdout, "Joined exit : %ld\n", index);
            return true;
        }
    next_i:;
    }
    return false;
}

void join_both(size_t index, bitset_vec_t **selected, adj_matrix_t *matrix) {
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
                fprintf(stdout, "mensaje de debug\n");
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
                fprintf(stdout, "mensaje de debug\n");
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
    bitset_insert_2((*selected)->bitsets[index_next], index);
    for (size_t i = 0; i < (*selected)->size; i++) {
        if (i != index_prev) {
            bitset_vec_push(new, (*selected)->bitsets[i]);
        }
    }
    fprintf(stdout, "Merged by :%ld\n", index);
    bitset_vec_free(selected);
    bitset_vec_copy(selected, new);
}