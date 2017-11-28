#include "oriented_graph.h"

OrientedGraph* oriented_graph_init(size_t item_size, oriented_graph_init_data_callback_f init_callback, oriented_graph_free_data_callback_f free_callback)
{
    return oriented_graph_init_with_capacity(item_size, 32, init_callback, free_callback);
}

OrientedGraph* oriented_graph_init_with_capacity(size_t item_size, size_t capacity, oriented_graph_init_data_callback_f init_callback, oriented_graph_free_data_callback_f free_callback)
{
    OrientedGraph* graph = memory_alloc(sizeof(OrientedGraph));
    graph->nodes_count = 0;
    graph->item_size = item_size;
    graph->capacity = capacity;
    graph->init_data_callback = init_callback;
    graph->free_data_callback = free_callback;

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
            new_node->in_edges = set_int_init();

            graph->nodes_count++;
            graph->nodes[i] = new_node;
            if(graph->init_data_callback != NULL)
                graph->init_data_callback(new_node);
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

    SetIntItem* id = (SetIntItem*)node->in_edges->head;
    GraphNodeBase* in_node = NULL;
    while(id != NULL) {
        in_node = oriented_graph_node(graph, id->value);
        set_int_remove(in_node->out_edges, id->value);

        id = (SetIntItem*)id->base.next;
    }

    id = (SetIntItem*)node->out_edges->head;
    GraphNodeBase* out_node = NULL;
    while(id != NULL) {
        out_node = oriented_graph_node(graph, id->value);
        set_int_remove(out_node->in_edges, id->value);

        id = (SetIntItem*)id->base.next;
    }

    if(graph->free_data_callback != NULL)
        graph->free_data_callback(node);
    graph->nodes[node_id] = NULL;
    set_int_free(&node->out_edges);
    set_int_free(&node->in_edges);
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
            if(graph->free_data_callback != NULL)
                graph->free_data_callback(graph->nodes[i]);

            set_int_free(&(graph->nodes[i]->out_edges));
            set_int_free(&(graph->nodes[i]->in_edges));
            memory_free(graph->nodes[i]);
            graph->nodes[i] = NULL;
        }
    }

    graph->nodes_count = 0;
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
    set_int_add(to->in_edges, from->id);
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

bool oriented_graph_node_is_in_cycle_by_id(OrientedGraph* graph, unsigned int id)
{
    NULL_POINTER_CHECK(graph, false);

    return oriented_graph_node_is_in_cycle(graph, oriented_graph_node(graph, id));
}

bool oriented_graph_node_is_in_cycle(OrientedGraph* graph, GraphNodeBase* node)
{
    NULL_POINTER_CHECK(graph, false);
    NULL_POINTER_CHECK(node, false);

    SetInt* ids = set_int_init();
    const unsigned int node_id = node->id;

    set_int_union(ids, node->out_edges);
    size_t previous_size;
    size_t current_size;

    do {
        if(set_int_contains(ids, node_id)) {
            set_int_free(&ids);
            return true;
        }

        previous_size = set_int_size(ids);
        _oriented_graph_expand_nodes(graph, ids);
        current_size = set_int_size(ids);
    } while(previous_size != current_size);

    set_int_free(&ids);
    return false;
}

void _oriented_graph_expand_nodes(OrientedGraph* graph, SetInt* layer)
{
    SetInt* expanded = set_int_init();
    SetIntItem* item = (SetIntItem*) layer->head;

    while(item != NULL) {
        const GraphNodeBase* node = oriented_graph_node(graph, (unsigned int) item->value);
        if(node == NULL)
            continue;

        set_int_union(expanded, node->out_edges);
        item = (SetIntItem*) item->base.next;
    }

    set_int_union(layer, expanded);
    set_int_free(&expanded);
}
