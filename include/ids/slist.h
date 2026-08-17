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
    The slist data struct stores a singly linked list
 */


#ifndef IDS_SLIST_H
#define IDS_SLIST_H


/*
    The node data to be placed in each slist element

    Accessible members:
        next -- the next node, or NULL at the end of the slist
 */
struct ids_slist_node
{
    struct ids_slist_node* next;
};

/*
    The data struct itself

    Accessible members:
        head -- the head node, or NULL when empty
        tail -- the tail node, or NULL when empty
 */
struct ids_slist
{
    struct ids_slist_node* head;
    struct ids_slist_node* tail;
};

/*
    The iterator struct is used to track the current position of a data struct iteration

    Accessible members:
        current_node -- the current element node being visited
 */
struct ids_slist_it
{
    struct ids_slist_node* current_node;
};


/*
    Initialize an slist data struct.  The data struct will be empty upon initialization
 */
inline void
Ids_Slist_Init (struct ids_slist* slist);

/*
    Reset an slist data struct to the initialized state
 */
inline void
Ids_Slist_Reset (struct ids_slist* slist);


/*
    Return the list head, or NULL when empty
 */
inline struct ids_slist_node*
Ids_Slist_Head (struct ids_slist* slist);

/*
    Return the list tail, or NULL when empty
 */
inline struct ids_slist_node*
Ids_Slist_Tail (struct ids_slist* slist);

/*
    Return non-zero if the slist is empty
 */
inline int
Ids_Slist_Empty (struct ids_slist* slist);


/*
    Insert a node after the specified node in the slist.  The existing node must belong to the
    specified non-empty slist
 */
inline void
Ids_Slist_Ins (struct ids_slist_node* restrict new_node,
               struct ids_slist_node* restrict existing_node,
               struct ids_slist*               slist);

/*
    Push a node at the head of the specified slist
 */
inline void
Ids_Slist_PushH (struct ids_slist_node* node, struct ids_slist* slist);

/*
    Pop the head node from an slist
 */
inline struct ids_slist_node*
Ids_Slist_PopH (struct ids_slist* slist);

/*
    Push a node at the tail of the specified slist
 */
inline void
Ids_Slist_PushT (struct ids_slist_node* node, struct ids_slist* slist);


/*
    Start an slist iteration from the specified node
 */
inline void
Ids_Slist_NBeginIt (struct ids_slist_node* node, struct ids_slist_it* it);

/*
    Start an slist iteration from the head node
 */
inline void
Ids_Slist_BeginIt (struct ids_slist* slist, struct ids_slist_it* it);

/*
    Iterate forwards
 */
inline void
Ids_Slist_ItFwd (struct ids_slist_it* it);

/*
    Return non-zero if the iterator is past the last node
 */
inline int
Ids_Slist_ItDone (struct ids_slist_it* it);


#include <ids/slist_inl.h>


#endif
