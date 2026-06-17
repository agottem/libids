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


inline void
Ids_Clist_Init (struct ids_clist* clist)
{
    clist->sentinel.next = &clist->sentinel;
    clist->sentinel.prev = &clist->sentinel;
}

inline void
Ids_Clist_Reset (struct ids_clist* clist)
{
    Ids_Clist_Init(clist);
}

inline struct ids_clist_node*
Ids_Clist_Head (struct ids_clist* clist)
{
    return clist->sentinel.next;
}

inline struct ids_clist_node*
Ids_Clist_Tail (struct ids_clist* clist)
{
    return clist->sentinel.prev;
}

inline struct ids_clist_node*
Ids_Clist_End (struct ids_clist* clist)
{
    return &clist->sentinel;
}

inline int
Ids_Clist_Empty (struct ids_clist* clist)
{
    return clist->sentinel.next == Ids_Clist_End(clist);
}

inline void
Ids_Clist_Splice (struct ids_clist_node* start,
                  struct ids_clist_node* end,
                  struct ids_clist_node* dest)
{
    start->prev->next = end->next;
    end->next->prev   = start->prev;
    dest->prev->next  = start;

    start->prev       = dest->prev;
    end->next         = dest;
    dest->prev        = end;
}

inline void
Ids_Clist_Ins (struct ids_clist_node* restrict new_node,
               struct ids_clist_node* restrict existing_node)
{
    struct ids_clist_node* prev = existing_node->prev;

    prev->next          = new_node;
    new_node->prev      = prev;
    new_node->next      = existing_node;
    existing_node->prev = new_node;
}

inline void
Ids_Clist_Del (struct ids_clist_node* node)
{
    struct ids_clist_node* next = node->next;
    struct ids_clist_node* prev = node->prev;

    prev->next = next;
    next->prev = prev;
}

inline void
Ids_Clist_PushH (struct ids_clist_node* node, struct ids_clist* clist)
{
    Ids_Clist_Ins(node, Ids_Clist_Head(clist));
}

inline struct ids_clist_node*
Ids_Clist_PopH (struct ids_clist* clist)
{
    struct ids_clist_node* head = Ids_Clist_Head(clist);

    assert(!Ids_Clist_Empty(clist) && "Attempting to pop an empty clist");

    Ids_Clist_Del(head);

    return head;
}

inline void
Ids_Clist_PushT (struct ids_clist_node* node, struct ids_clist* clist)
{
    Ids_Clist_Ins(node, Ids_Clist_End(clist));
}

inline struct ids_clist_node*
Ids_Clist_PopT (struct ids_clist* clist)
{
    struct ids_clist_node* tail = Ids_Clist_Tail(clist);

    assert(!Ids_Clist_Empty(clist) && "Attempting to pop an empty clist");

    Ids_Clist_Del(tail);

    return tail;
}

inline void
Ids_Clist_NBeginIt (struct ids_clist_node* node, struct ids_clist_it* it)
{
    it->current_node = node;
}

inline void
Ids_Clist_BeginIt (struct ids_clist* clist, struct ids_clist_it* it)
{
    Ids_Clist_NBeginIt(Ids_Clist_Head(clist), it);
}

inline void
Ids_Clist_RBeginIt (struct ids_clist* clist, struct ids_clist_it* it)
{
    Ids_Clist_NBeginIt(Ids_Clist_Tail(clist), it);
}

inline void
Ids_Clist_ItFwd (struct ids_clist_it* it)
{
    it->current_node = it->current_node->next;
}

inline void
Ids_Clist_ItBack (struct ids_clist_it* it)
{
    it->current_node = it->current_node->prev;
}

inline int
Ids_Clist_ItDone (struct ids_clist* clist, struct ids_clist_it* it)
{
    return it->current_node == Ids_Clist_End(clist);
}
