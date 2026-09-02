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


#include <ids/rbtree.h>


extern enum ids_rbtree_color
Ids_RbTree_Color (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_NodeMin (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_NodeMax (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_NodeNext (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_NodePrev (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_PostFirst (struct ids_rbtree_node* node);

extern struct ids_rbtree_node*
Ids_RbTree_PostNext (struct ids_rbtree_node* node);

extern void
Ids_RbTree_RotateLeft (struct ids_rbtree_node* node, struct ids_rbtree* rbtree);

extern void
Ids_RbTree_RotateRight (struct ids_rbtree_node* node, struct ids_rbtree* rbtree);

extern void
Ids_RbTree_FixAdd (struct ids_rbtree_node* node, struct ids_rbtree* rbtree);

extern void
Ids_RbTree_Replace (struct ids_rbtree_node* node,
                    struct ids_rbtree_node* replacement,
                    struct ids_rbtree*      rbtree);

extern void
Ids_RbTree_FixDel (struct ids_rbtree_node* node,
                   struct ids_rbtree_node* parent,
                   struct ids_rbtree*      rbtree);


extern void
Ids_RbTree_Init (struct ids_rbtree* rbtree);

extern void
Ids_RbTree_Reset (struct ids_rbtree* rbtree);

extern int
Ids_RbTree_Empty (struct ids_rbtree* rbtree);

extern struct ids_rbtree_node*
Ids_RbTree_Root (struct ids_rbtree* rbtree);

extern struct ids_rbtree_node*
Ids_RbTree_Min (struct ids_rbtree* rbtree);

extern struct ids_rbtree_node*
Ids_RbTree_Max (struct ids_rbtree* rbtree);


extern struct ids_rbtree_node*
Ids_RbTree_Find (void* id, struct ids_rbtree* rbtree, ids_rbtree_cmp_t* cmp_func, void* user_data);

extern struct ids_rbtree_node*
Ids_RbTree_LowerBound (void*              id,
                       struct ids_rbtree* rbtree,
                       ids_rbtree_cmp_t*  cmp_func,
                       void*              user_data);

extern struct ids_rbtree_node*
Ids_RbTree_UpperBound (void*              id,
                       struct ids_rbtree* rbtree,
                       ids_rbtree_cmp_t*  cmp_func,
                       void*              user_data);


extern struct ids_rbtree_node*
Ids_RbTree_Add (void*                   id,
                struct ids_rbtree_node* node,
                struct ids_rbtree*      rbtree,
                ids_rbtree_cmp_t*       cmp_func,
                void*                   user_data);

extern void
Ids_RbTree_Del (struct ids_rbtree_node* node, struct ids_rbtree* rbtree);


extern void
Ids_RbTree_NBeginIt (struct ids_rbtree_node* node, struct ids_rbtree_it* it);

extern void
Ids_RbTree_BeginIt (struct ids_rbtree* rbtree, struct ids_rbtree_it* it);

extern void
Ids_RbTree_RBeginIt (struct ids_rbtree* rbtree, struct ids_rbtree_it* it);

extern void
Ids_RbTree_ItFwd (struct ids_rbtree_it* it);

extern void
Ids_RbTree_ItBack (struct ids_rbtree_it* it);

extern int
Ids_RbTree_ItDone (struct ids_rbtree_it* it);


extern void
Ids_RbTree_BeginPostIt (struct ids_rbtree* rbtree, struct ids_rbtree_post_it* it);

extern void
Ids_RbTree_PostItFwd (struct ids_rbtree_post_it* it);

extern int
Ids_RbTree_PostItDone (struct ids_rbtree_post_it* it);
