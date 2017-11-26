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

        for(unsigned int i = 0; i < graph->capacity; i++)
            graph->nodes[i] = nodes[i];
        memory_free(nodes);

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
            break;
        }
    }

    if(!assigned)
        LOG_WARNING("Graph internal error in insertion.");

    return new_node;
}

void oriented_graph_remove_node(OrientedGraph* graph, unsigned int node_id)
{
    NULL_POINTER_CHECK(graph, );

    GraphNodeBase* node = oriented_graph_node(graph, node_id);
    if(node == NULL)
        return;

    graph->nodes[node_id] = NULL;
    set_int_free(&(node->out_edges));
    memory_free(node);

    graph->nodes_count--;
}

GraphNodeBase* oriented_graph_node(OrientedGraph* graph, unsigned int node_id)
{
    NULL_POINTER_CHECK(graph, NULL);

    if(node_id >= graph->capacity)
        return NULL;

    return graph->nodes[node_id];
}

void oriented_graph_clear(OrientedGraph* graph)
{
    for(unsigned int i = 0; i < graph->capacity; i++) {
        if(graph->nodes[i] != NULL) {
            // TODO add free data callback
            set_int_free(&(graph->nodes[i]->out_edges));
            memory_free(graph->nodes[i]);
            graph->nodes[i] = NULL;
        }
    }

    graph->nodes_count = 0;
    graph->capacity = 0;
}

void oriented_graph_print(OrientedGraph* graph)
{
    printf("OrientedGraph(\n");
    for(unsigned int i = 0; i < graph->capacity; i++) {
        if(graph->nodes[i] == NULL)
            continue;

        const GraphNodeBase* node = graph->nodes[i];
        printf("Node %d -> ", node->id);
        set_int_print(node->out_edges);
        printf("\n");
    }

    printf(")\n");
}

void oriented_graph_connect_nodes(OrientedGraph* graph, GraphNodeBase* from, GraphNodeBase* to)
{
    NULL_POINTER_CHECK(graph, );
    NULL_POINTER_CHECK(from, );
    NULL_POINTER_CHECK(to, );

    set_int_add(from->out_edges, to->id);
}

void oriented_graph_connect_nodes_by_ids(OrientedGraph* graph, unsigned int from, unsigned int to)
{
    NULL_POINTER_CHECK(graph, );

    GraphNodeBase* from_node = oriented_graph_node(graph, from);
    GraphNodeBase* to_node = oriented_graph_node(graph, to);
    if(from_node == NULL || to_node == NULL)
        return;

    oriented_graph_connect_nodes(graph, from_node, to_node);
}
