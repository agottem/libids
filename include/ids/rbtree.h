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
    The rbtree data struct stores a unique, ordered set of nodes
 */


#ifndef IDS_RBTREE_H
#define IDS_RBTREE_H


enum ids_rbtree_color
{
    ids_rbtree_color_red,
    ids_rbtree_color_black,
};


/*
    The node data to be placed in each rbtree element
 */
struct ids_rbtree_node
{
    struct ids_rbtree_node* parent;
    struct ids_rbtree_node* left;
    struct ids_rbtree_node* right;

    enum ids_rbtree_color   color;
};

/*
    The rbtree data struct itself

    Accessible members:
        root -- the root node, or NULL when empty
 */
struct ids_rbtree
{
    struct ids_rbtree_node* root;
};

/*
    The iterator struct used to visit nodes in key order

    Accessible members:
        node -- the current element node being visited
 */
struct ids_rbtree_it
{
    struct ids_rbtree_node* node;
};

/*
    The iterator struct used to visit nodes in post-order

    Accessible members:
        node -- the current element node being visited
 */
struct ids_rbtree_post_it
{
    struct ids_rbtree_node* node;
    struct ids_rbtree_node* next;
};


/*
    The required type for a user-defined rbtree cmp function.  Return a value less than zero if
    id sorts before node, zero if id matches node, or greater than zero if id sorts after node
 */
typedef int
ids_rbtree_cmp_t (void* id, struct ids_rbtree_node* node, void* user_data);


/*
    Initialize an rbtree data struct.  The data struct will be empty upon initialization
 */
inline void
Ids_RbTree_Init (struct ids_rbtree* rbtree);

/*
    Reset an rbtree data struct to the initialized state
 */
inline void
Ids_RbTree_Reset (struct ids_rbtree* rbtree);


/*
    Return non-zero if the rbtree is empty
 */
inline int
Ids_RbTree_Empty (struct ids_rbtree* rbtree);

/*
    Return the root node, or NULL when empty
 */
inline struct ids_rbtree_node*
Ids_RbTree_Root (struct ids_rbtree* rbtree);

/*
    Return the first node in key order, or NULL when empty
 */
inline struct ids_rbtree_node*
Ids_RbTree_Min (struct ids_rbtree* rbtree);

/*
    Return the last node in key order, or NULL when empty
 */
inline struct ids_rbtree_node*
Ids_RbTree_Max (struct ids_rbtree* rbtree);


/*
    Find a node matching id.  NULL is returned if no node was found
 */
inline struct ids_rbtree_node*
Ids_RbTree_Find (struct ids_rbtree* rbtree, void* id, ids_rbtree_cmp_t* cmp_func, void* user_data);

/*
    Return the first node which does not sort before id, or NULL if no such node exists
 */
inline struct ids_rbtree_node*
Ids_RbTree_LowerBound (struct ids_rbtree* rbtree,
                       void*              id,
                       ids_rbtree_cmp_t*  cmp_func,
                       void*              user_data);

/*
    Return the first node which sorts after id, or NULL if no such node exists
 */
inline struct ids_rbtree_node*
Ids_RbTree_UpperBound (struct ids_rbtree* rbtree,
                       void*              id,
                       ids_rbtree_cmp_t*  cmp_func,
                       void*              user_data);


/*
    Add a node to the rbtree.  NULL is returned if the node was added.  If an equivalent node
    already exists, that node is returned and the specified node is not modified
 */
inline struct ids_rbtree_node*
Ids_RbTree_Add (struct ids_rbtree*      rbtree,
                void*                   id,
                struct ids_rbtree_node* node,
                ids_rbtree_cmp_t*       cmp_func,
                void*                   user_data);

/*
    Delete the specified node from the rbtree
 */
inline void
Ids_RbTree_Del (struct ids_rbtree* rbtree, struct ids_rbtree_node* node);


/*
    Start an iteration from the specified node
 */
inline void
Ids_RbTree_NBeginIt (struct ids_rbtree_node* node, struct ids_rbtree_it* it);

/*
    Start an iteration from the first node in key order
 */
inline void
Ids_RbTree_BeginIt (struct ids_rbtree* rbtree, struct ids_rbtree_it* it);

/*
    Start an iteration from the last node in key order
 */
inline void
Ids_RbTree_RBeginIt (struct ids_rbtree* rbtree, struct ids_rbtree_it* it);

/*
    Iterate forwards in key order
 */
inline void
Ids_RbTree_ItFwd (struct ids_rbtree_it* it);

/*
    Iterate backwards in key order
 */
inline void
Ids_RbTree_ItBack (struct ids_rbtree_it* it);

/*
    Return non-zero if the iterator is past the last node
 */
inline int
Ids_RbTree_ItDone (struct ids_rbtree_it* it);


/*
    Start a post-order iteration.  After advancing, the previously visited node may be freed.
    If nodes are freed during iteration, the rbtree must be reset after iteration completes
 */
inline void
Ids_RbTree_BeginPostIt (struct ids_rbtree* rbtree, struct ids_rbtree_post_it* it);

/*
    Iterate forwards in post-order
 */
inline void
Ids_RbTree_PostItFwd (struct ids_rbtree_post_it* it);

/*
    Return non-zero if the post-order iterator is past the last node
 */
inline int
Ids_RbTree_PostItDone (struct ids_rbtree_post_it* it);


#include <ids/rbtree_inl.h>


#endif
