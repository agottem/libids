/*
    libids is licensed under the simplified BSD license:

    Copyright 2026, Andrew Gottemoller
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer:

    Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other materials
    provided with the distribution.

    Neither the name Andrew Gottemoller nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific prior written
    permission.
 */


#include <assert.h>
#include <stdint.h>
#include <stdlib.h>


inline size_t
Ids_Heap_ParentIndex (size_t index)
{
    return (index - 1) / 2;
}

inline size_t
Ids_Heap_LeftIndex (size_t index)
{
    return index * 2 + 1;
}

inline size_t
Ids_Heap_RightIndex (size_t index)
{
    return index * 2 + 2;
}

inline void
Ids_Heap_PlaceNode (struct ids_heap* heap, struct ids_heap_node* node, size_t index)
{
    heap->nodes[index] = node;
    node->index        = index;
}

inline void
Ids_Heap_SiftUp (struct ids_heap*      heap,
                 struct ids_heap_node* node,
                 size_t                start_index,
                 ids_heap_cmp_t*       cmp_func,
                 void*                 user_data)
{
    size_t index = start_index;
    while(index > 0)
    {
        size_t                parent_index = Ids_Heap_ParentIndex(index);
        struct ids_heap_node* parent       = heap->nodes[parent_index];
        if((*cmp_func)(node, parent, user_data) >= 0)
            break;

        Ids_Heap_PlaceNode(heap, parent, index);
        index = parent_index;
    }

    Ids_Heap_PlaceNode(heap, node, index);
}

inline void
Ids_Heap_SiftDown (struct ids_heap*      heap,
                   struct ids_heap_node* node,
                   size_t                start_index,
                   ids_heap_cmp_t*       cmp_func,
                   void*                 user_data)
{
    size_t index      = start_index;
    size_t node_count = heap->node_count;
    size_t max_index  = node_count / 2;
    while(index < max_index)
    {
        size_t left_index  = Ids_Heap_LeftIndex(index);
        size_t right_index = Ids_Heap_RightIndex(index);
        size_t child_index = left_index;
        if(right_index < node_count &&
           (*cmp_func)(heap->nodes[right_index], heap->nodes[left_index], user_data) < 0)
        {
            child_index = right_index;
        }

        struct ids_heap_node* child = heap->nodes[child_index];
        if((*cmp_func)(node, child, user_data) <= 0)
            break;

        Ids_Heap_PlaceNode(heap, child, index);
        index = child_index;
    }

    Ids_Heap_PlaceNode(heap, node, index);
}

inline void
Ids_Heap_Fix (struct ids_heap*      heap,
              struct ids_heap_node* node,
              size_t                index,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data)
{
    if(index > 0 &&
       (*cmp_func)(node, heap->nodes[Ids_Heap_ParentIndex(index)], user_data) < 0)
        Ids_Heap_SiftUp(heap, node, index, cmp_func, user_data);
    else
        Ids_Heap_SiftDown(heap, node, index, cmp_func, user_data);
}


inline void
Ids_Heap_Init (struct ids_heap* heap, size_t capacity, struct ids_heap_node** nodes)
{
    assert(capacity > 0 && "Heap capacity can't be zero");

    heap->node_count = 0;
    heap->capacity   = capacity;
    heap->nodes      = nodes;
}

inline enum ids_err
Ids_Heap_Create (struct ids_heap* heap, size_t capacity)
{
    struct ids_heap_node** nodes = calloc(capacity, sizeof(struct ids_heap_node*));
    if(nodes == NULL)
        return ids_err_mem;

    Ids_Heap_Init(heap, capacity, nodes);

    return ids_err_none;
}

inline void
Ids_Heap_Destroy (struct ids_heap* heap)
{
    free(heap->nodes);
}

inline void
Ids_Heap_Reset (struct ids_heap* heap)
{
    heap->node_count = 0;
}

inline size_t
Ids_Heap_Count (const struct ids_heap* heap)
{
    return heap->node_count;
}

inline size_t
Ids_Heap_Capacity (const struct ids_heap* heap)
{
    return heap->capacity;
}

inline int
Ids_Heap_Empty (const struct ids_heap* heap)
{
    return heap->node_count == 0;
}

inline int
Ids_Heap_Full (const struct ids_heap* heap)
{
    return heap->node_count == heap->capacity;
}

inline struct ids_heap_node*
Ids_Heap_Root (const struct ids_heap* heap)
{
    assert(!Ids_Heap_Empty(heap) && "Heap must not be empty");
    return heap->nodes[0];
}

inline void
Ids_Heap_Add (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data)
{
    assert(!Ids_Heap_Full(heap) && "Attempting to add to a full heap");

    size_t index = heap->node_count++;
    Ids_Heap_SiftUp(heap, node, index, cmp_func, user_data);
}

inline struct ids_heap_node*
Ids_Heap_Pop (struct ids_heap* heap, ids_heap_cmp_t* cmp_func, void* user_data)
{
    assert(!Ids_Heap_Empty(heap) && "Attempting to pop an empty heap");

    struct ids_heap_node* node = Ids_Heap_Root(heap);
    heap->node_count--;
    if(heap->node_count > 0)
        Ids_Heap_SiftDown(heap, heap->nodes[heap->node_count], 0, cmp_func, user_data);

    return node;
}

inline void
Ids_Heap_Del (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data)
{
    size_t index = node->index;
    assert(index < heap->node_count && heap->nodes[index] == node &&
           "Node must belong to the heap");

    heap->node_count--;
    if(index != heap->node_count)
        Ids_Heap_Fix(heap, heap->nodes[heap->node_count], index, cmp_func, user_data);
}

inline void
Ids_Heap_Update (struct ids_heap*      heap,
                 struct ids_heap_node* node,
                 ids_heap_cmp_t*       cmp_func,
                 void*                 user_data)
{
    assert(node->index < heap->node_count && heap->nodes[node->index] == node &&
           "Node must belong to the heap");

    Ids_Heap_Fix(heap, node, node->index, cmp_func, user_data);
}
