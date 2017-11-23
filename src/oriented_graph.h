#ifndef ORIENTEDGRAPH_H
#define ORIENTEDGRAPH_H

#include <stdlib.h>
#include "set_int.h"

typedef struct {
    unsigned int id;
    SetInt* out_edges;
} GraphNodeBase;

typedef struct {
    size_t capacity;
    size_t nodes_count;
    size_t item_size;
    GraphNodeBase** nodes;
} OrientedGraph;

OrientedGraph* oriented_graph_init(size_t item_size);
void oriented_graph_free(OrientedGraph** graph);

GraphNodeBase* oriented_graph_new_node(OrientedGraph* graph);
void oriented_graph_remove_node(OrientedGraph* graph, unsigned int node_id);

GraphNodeBase* oriented_graph_node(OrientedGraph* graph, unsigned int node_id);
void oriented_graph_clear(OrientedGraph* graph);

#endif // ORIENTEDGRAPH_H
