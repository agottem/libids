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


#include <ids/heap.h>


extern size_t
Ids_Heap_ParentIndex (size_t index);

extern size_t
Ids_Heap_LeftIndex (size_t index);

extern size_t
Ids_Heap_RightIndex (size_t index);

extern void
Ids_Heap_PlaceNode (struct ids_heap* heap, struct ids_heap_node* node, size_t index);

extern void
Ids_Heap_SiftUp (struct ids_heap*      heap,
                 struct ids_heap_node* node,
                 size_t                index,
                 ids_heap_cmp_t*       cmp_func,
                 void*                 user_data);

extern void
Ids_Heap_SiftDown (struct ids_heap*      heap,
                   struct ids_heap_node* node,
                   size_t                index,
                   ids_heap_cmp_t*       cmp_func,
                   void*                 user_data);

extern void
Ids_Heap_Fix (struct ids_heap*      heap,
              struct ids_heap_node* node,
              size_t                index,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data);


extern void
Ids_Heap_Init (struct ids_heap* heap, size_t capacity, struct ids_heap_node** nodes);

extern enum ids_err
Ids_Heap_Create (struct ids_heap* heap, size_t capacity);

extern void
Ids_Heap_Destroy (struct ids_heap* heap);

extern void
Ids_Heap_Reset (struct ids_heap* heap);

extern size_t
Ids_Heap_Count (const struct ids_heap* heap);

extern size_t
Ids_Heap_Capacity (const struct ids_heap* heap);

extern int
Ids_Heap_Empty (const struct ids_heap* heap);

extern int
Ids_Heap_Full (const struct ids_heap* heap);

extern struct ids_heap_node*
Ids_Heap_Root (const struct ids_heap* heap);

extern void
Ids_Heap_Add (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data);

extern struct ids_heap_node*
Ids_Heap_Pop (struct ids_heap* heap, ids_heap_cmp_t* cmp_func, void* user_data);

extern void
Ids_Heap_Del (struct ids_heap*      heap,
              struct ids_heap_node* node,
              ids_heap_cmp_t*       cmp_func,
              void*                 user_data);

extern void
Ids_Heap_Update (struct ids_heap*      heap,
                 struct ids_heap_node* node,
                 ids_heap_cmp_t*       cmp_func,
                 void*                 user_data);
