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
#include <stddef.h>


inline void
Ids_Slist_Init (struct ids_slist* slist)
{
    slist->head = NULL;
    slist->tail = NULL;
}

inline void
Ids_Slist_Reset (struct ids_slist* slist)
{
    Ids_Slist_Init(slist);
}

inline struct ids_slist_node*
Ids_Slist_Head (struct ids_slist* slist)
{
    return slist->head;
}

inline struct ids_slist_node*
Ids_Slist_Tail (struct ids_slist* slist)
{
    return slist->tail;
}

inline int
Ids_Slist_Empty (struct ids_slist* slist)
{
    return slist->head == NULL;
}

inline void
Ids_Slist_Ins (struct ids_slist*               slist,
               struct ids_slist_node* restrict new_node,
               struct ids_slist_node* restrict existing_node)
{
    new_node->next      = existing_node->next;
    existing_node->next = new_node;

    if(slist->tail == existing_node)
        slist->tail = new_node;
}

inline void
Ids_Slist_SpliceH (struct ids_slist* restrict source, struct ids_slist* restrict dest)
{
    assert(source != dest && "Attempting to splice an slist into itself");

    if(source->head == NULL)
        return;

    source->tail->next = dest->head;
    dest->head         = source->head;

    if(dest->tail == NULL)
        dest->tail = source->tail;

    Ids_Slist_Reset(source);
}

inline void
Ids_Slist_SpliceT (struct ids_slist* restrict source, struct ids_slist* restrict dest)
{
    assert(source != dest && "Attempting to splice an slist into itself");

    if(source->head == NULL)
        return;

    if(dest->tail == NULL)
        dest->head = source->head;
    else
        dest->tail->next = source->head;

    dest->tail = source->tail;

    Ids_Slist_Reset(source);
}

inline void
Ids_Slist_PushH (struct ids_slist* slist, struct ids_slist_node* node)
{
    node->next  = slist->head;
    slist->head = node;

    if(slist->tail == NULL)
        slist->tail = node;
}

inline struct ids_slist_node*
Ids_Slist_PopH (struct ids_slist* slist)
{
    assert(!Ids_Slist_Empty(slist) && "Attempting to pop an empty slist");

    struct ids_slist_node* head = slist->head;

    slist->head = head->next;
    if(slist->head == NULL)
        slist->tail = NULL;

    return head;
}

inline void
Ids_Slist_PushT (struct ids_slist* slist, struct ids_slist_node* node)
{
    node->next = NULL;

    if(slist->tail == NULL)
        slist->head = node;
    else
        slist->tail->next = node;

    slist->tail = node;
}

inline void
Ids_Slist_NBeginIt (struct ids_slist_node* node, struct ids_slist_it* it)
{
    it->node = node;
}

inline void
Ids_Slist_BeginIt (struct ids_slist* slist, struct ids_slist_it* it)
{
    Ids_Slist_NBeginIt(Ids_Slist_Head(slist), it);
}

inline void
Ids_Slist_ItFwd (struct ids_slist_it* it)
{
    it->node = it->node->next;
}

inline int
Ids_Slist_ItDone (struct ids_slist_it* it)
{
    return it->node == NULL;
}
