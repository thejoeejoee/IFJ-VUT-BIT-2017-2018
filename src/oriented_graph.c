#include "oriented_graph.h"

OrientedGraph* oriented_graph_init(size_t item_size)
{
    OrientedGraph* graph = memory_alloc(sizeof(OrientedGraph));
    graph->nodes_count = 0;
    graph->item_size = item_size;
    graph->capacity = 32;

    graph->nodes = memory_alloc(sizeof(GraphNodeBase*) * graph->capacity);
    for(unsigned int i = 0; i < graph->capacity; i++)
        graph->nodes[i] = NULL;

    return graph;
}

void oriented_graph_free(OrientedGraph** graph)
{
    NULL_POINTER_CHECK(graph, );
    NULL_POINTER_CHECK(*graph, );

    oriented_graph_clear(*graph);

    memory_free((*graph)->nodes);
    memory_free(*graph);
    *graph = NULL;
}

GraphNodeBase* oriented_graph_new_node(OrientedGraph* graph)
{
    NULL_POINTER_CHECK(graph, NULL);

    // resize nodes container
    if(graph->nodes_count >= graph->capacity) {
        GraphNodeBase** nodes = graph->nodes;
        graph->nodes = memory_alloc(sizeof(GraphNodeBase*) * graph->capacity * 2);
        memmove(graph->nodes, nodes, sizeof(GraphNodeBase*) * graph->capacity);

        for(unsigned int i = graph->capacity; i < graph->capacity * 2; i++)
            graph->nodes[i] = NULL;

        graph->capacity *= 2;
    }

    bool assigned = false;
    GraphNodeBase* new_node = memory_alloc(graph->item_size);

    for(unsigned int i = 0; i < graph->capacity; i++) {
        if(graph->nodes[i] == NULL) {
            assigned = true;
            new_node->id = i;
            new_node->out_edges = set_int_init();

            graph->nodes_count++;
            graph->nodes[i] = new_node;
        }
    }

    if(!assigned)
        LOG_WARNING("Graph internal error in insertion.");

    return new_node;
}

void oriented_graph_remove_node(OrientedGraph* graph, unsigned int node_id)
{

    graph->nodes_count--;
}

GraphNodeBase* oriented_graph_node(OrientedGraph* graph, unsigned int node_id)
{
//    for(unsigned in)
    return NULL;
}

void oriented_graph_clear(OrientedGraph* graph)
{
    for(unsigned int i = 0; i < graph->capacity; i++) {
        if(graph->nodes[i] != NULL) {
            memory_free(graph->nodes[i]);
            graph->nodes[i] = NULL;
        }
    }

    graph->nodes_count = 0;
    graph->capacity = 0;
}
