/*
    libids library is licensed under the simplified BSD license:

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
    The clist data struct stores a circular doubly linked list
 */


#ifndef IDS_CLIST_H
#define IDS_CLIST_H


/*
    The node data to be placed in each clist element

    Accessible members:
        next -- the next node
        prev -- the previous node
 */
struct ids_clist_node
{
    struct ids_clist_node* next;
    struct ids_clist_node* prev;
};

/*
    The data struct itself

    Accessible members:
        sentinel -- access sentinel.next or sentinel.prev for the head or tail of the clist
 */
struct ids_clist
{
    struct ids_clist_node sentinel;
};

/*
    The iterator struct is used to track the current position of a data struct iteration

    Accessible members:
        current_node -- the current element node being visited
 */
struct ids_clist_it
{
    struct ids_clist_node* current_node;
};


/*
    Initialize a clist data struct.  The data struct will be empty upon initialization
 */
inline void
Ids_Clist_Init (struct ids_clist* clist);

/*
    Reset a clist data struct to the initialized state
 */
inline void
Ids_Clist_Reset (struct ids_clist* clist);


/*
    Return the list head
 */
inline struct ids_clist_node*
Ids_Clist_Head (struct ids_clist* clist);

/*
    Return the list tail
 */
inline struct ids_clist_node*
Ids_Clist_Tail (struct ids_clist* clist);

/*
    Return the end node (which is not a valid node)
 */
inline struct ids_clist_node*
Ids_Clist_End (struct ids_clist* clist);

/*
    Return non-zero if the clist is empty
 */
inline int
Ids_Clist_Empty (struct ids_clist* clist);


/*
    Place the specified range from a clist before the destination node
 */
inline void
Ids_Clist_Splice (struct ids_clist_node* start,
                  struct ids_clist_node* end,
                  struct ids_clist_node* dest);

/*
    Insert a node before the specified node in the clist
 */
inline void
Ids_Clist_Ins (struct ids_clist_node* restrict new_node,
               struct ids_clist_node* restrict existing_node);

/*
    Delete the specified node from its clist
 */
inline void
Ids_Clist_Del (struct ids_clist_node* node);

/*
    Push a node at the head of the specified clist
 */
inline void
Ids_Clist_PushH (struct ids_clist_node* node, struct ids_clist* clist);

/*
    Pop the head node from a clist
 */
inline struct ids_clist_node*
Ids_Clist_PopH (struct ids_clist* clist);

/*
    Push a node at the tail of the specified clist
 */
inline void
Ids_Clist_PushT (struct ids_clist_node* node, struct ids_clist* clist);

/*
    Pop the tail node from a clist
 */
inline struct ids_clist_node*
Ids_Clist_PopT (struct ids_clist* clist);


/*
    Start a clist iteration from the specified node
 */
inline void
Ids_Clist_NBeginIt (struct ids_clist_node* node, struct ids_clist_it* it);

/*
    Start a clist iteration from the head node
 */
inline void
Ids_Clist_BeginIt (struct ids_clist* clist, struct ids_clist_it* it);

/*
    Start a clist iteration from the tail node
 */
inline void
Ids_Clist_RBeginIt (struct ids_clist* clist, struct ids_clist_it* it);

/*
    Iterate forwards
 */
inline void
Ids_Clist_ItFwd (struct ids_clist_it* it);

/*
    Iterate backwards
 */
inline void
Ids_Clist_ItBack (struct ids_clist_it* it);

/*
    Return non-zero if the iterator is at the end node
 */
inline int
Ids_Clist_ItDone (struct ids_clist* clist, struct ids_clist_it* it);


#include <ids/clist_inl.h>


#endif
