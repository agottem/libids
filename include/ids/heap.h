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

/*
    The heap data struct stores a fixed-capacity, indexed binary heap of node pointers.
    Nodes and their enclosing objects remain owned by the caller and never move.
    Equal priorities are permitted; their extraction order is unspecified.
    Concurrent access involving mutations requires external synchronization.
 */


#ifndef IDS_HEAP_H
#define IDS_HEAP_H


#include <ids/err.h>

#include <stddef.h>


/*
    The node data to be placed in each heap element.  A node may belong to only one heap
    at a time.  No node initialization is required; removed node indexes are unspecified
 */
struct ids_heap_node
{
    size_t index;
};

/*
    The heap data struct itself

    Accessible members:
        node_count -- the number of nodes currently stored
        capacity -- the number of pointers the storage can hold
        nodes -- the pointer storage; only the first node_count entries are occupied
 */
struct ids_heap
{
    size_t                 node_count;
    size_t                 capacity;
    struct ids_heap_node** nodes;
};


/*
    Return less than zero if left sorts before right, zero for equivalent priorities, or
    greater than zero if left sorts after right.  The root sorts first.  All operations
    on a heap must use a consistent ordering.  Comparators must not modify the heap
 */
typedef int
ids_heap_cmp_t (const struct ids_heap_node* left,
                const struct ids_heap_node* right,
                void*                       user_data);


/*
    Initialize an empty heap with caller-provided storage for capacity pointers.  Storage
    must remain valid while in use.  Zero capacity permits NULL storage.  O(1)
 */
inline void
Ids_Heap_Init (struct ids_heap* heap, size_t capacity, struct ids_heap_node** nodes);

/*
    Allocate pointer storage and initialize an empty heap.  Zero capacity succeeds without
    allocation.  On allocation failure or size overflow, return ids_err_mem and leave heap
    unchanged.  Nodes themselves are never allocated
 */
inline enum ids_err
Ids_Heap_Create (struct ids_heap* heap, size_t capacity);

/*
    Free pointer storage for a successfully created heap.  Enclosing objects are not freed.
    Do not call for heaps initialized with caller-provided storage
 */
inline void
Ids_Heap_Destroy (struct ids_heap* heap);

/*
    Discard all membership while retaining storage.  Node indexes are not cleared.  O(1)
 */
inline void
Ids_Heap_Reset (struct ids_heap* heap);


/*
    Return the number of stored nodes.  O(1)
 */
inline size_t
Ids_Heap_Count (const struct ids_heap* heap);

/*
    Return the storage capacity.  O(1)
 */
inline size_t
Ids_Heap_Capacity (const struct ids_heap* heap);

/*
    Return non-zero if empty.  O(1)
 */
inline int
Ids_Heap_Empty (const struct ids_heap* heap);

/*
    Return non-zero if full.  O(1)
 */
inline int
Ids_Heap_Full (const struct ids_heap* heap);

/*
    Return the first node under the comparator ordering, or NULL when empty.  O(1)
 */
inline struct ids_heap_node*
Ids_Heap_Root (const struct ids_heap* heap);


/*
    Insert a node which is not currently in a heap.  The heap must not be full.  O(log n)
 */
inline void
Ids_Heap_Add (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data);

/*
    Remove and return the root.  The heap must not be empty.  O(log n)
 */
inline struct ids_heap_node*
Ids_Heap_Pop (struct ids_heap* heap, ids_heap_cmp_t* cmp_func, void* user_data);

/*
    Remove a node which must belong to the supplied heap.  O(log n)
 */
inline void
Ids_Heap_Del (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data);

/*
    Repair ordering after changing one member node's priority.  Call before any further
    heap operation.  Other nodes' priorities and the comparator ordering must not change.
    The node must belong to the supplied heap.  O(log n)
 */
inline void
Ids_Heap_Update (struct ids_heap*      heap,
                 struct ids_heap_node* node,
                 ids_heap_cmp_t*       cmp_func,
                 void*                 user_data);


#include <ids/heap_inl.h>


#endif
