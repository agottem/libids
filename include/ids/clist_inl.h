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
IDS_CList_Init (struct ids_clist* clist)
{
    clist->sentinel.next = &clist->sentinel;
    clist->sentinel.prev = &clist->sentinel;
}

inline void
IDS_CList_Reset (struct ids_clist* clist)
{
    IDS_CList_Init(clist);
}

inline struct ids_clist_node*
IDS_CList_Head (struct ids_clist* clist)
{
    return clist->sentinel.next;
}

inline struct ids_clist_node*
IDS_CList_Tail (struct ids_clist* clist)
{
    return clist->sentinel.prev;
}

inline struct ids_clist_node*
IDS_CList_End (struct ids_clist* clist)
{
    return &clist->sentinel;
}

inline int
IDS_CList_Empty (struct ids_clist* clist)
{
    return clist->sentinel.next == IDS_CList_End(clist);
}

inline void
IDS_CList_Splice (struct ids_clist_node* start,
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
IDS_CList_Ins (struct ids_clist_node* restrict new_node,
               struct ids_clist_node* restrict existing_node)
{
    struct ids_clist_node* prev = existing_node->prev;

    prev->next          = new_node;
    new_node->prev      = prev;
    new_node->next      = existing_node;
    existing_node->prev = new_node;
}

inline void
IDS_CList_Del (struct ids_clist_node* node)
{
    struct ids_clist_node* next = node->next;
    struct ids_clist_node* prev = node->prev;

    prev->next = next;
    next->prev = prev;
}

inline void
IDS_CList_PushH (struct ids_clist_node* node, struct ids_clist* clist)
{
    IDS_CList_Ins(node, IDS_CList_Head(clist));
}

inline struct ids_clist_node*
IDS_CList_PopH (struct ids_clist* clist)
{
    struct ids_clist_node* head = IDS_CList_Head(clist);

    assert(!IDS_CList_Empty(clist) && "Attempting to pop an empty clist");

    IDS_CList_Del(head);

    return head;
}

inline void
IDS_CList_PushT (struct ids_clist_node* node, struct ids_clist* clist)
{
    IDS_CList_Ins(node, IDS_CList_End(clist));
}

inline struct ids_clist_node*
IDS_CList_PopT (struct ids_clist* clist)
{
    struct ids_clist_node* tail = IDS_CList_Tail(clist);

    assert(!IDS_CList_Empty(clist) && "Attempting to pop an empty clist");

    IDS_CList_Del(tail);

    return tail;
}

inline void
IDS_CList_NBeginIt (struct ids_clist_node* node, struct ids_clist_it* it)
{
    it->current_node = node;
}

inline void
IDS_CList_BeginIt (struct ids_clist* clist, struct ids_clist_it* it)
{
    IDS_CList_NBeginIt(IDS_CList_Head(clist), it);
}

inline void
IDS_CList_RBeginIt (struct ids_clist* clist, struct ids_clist_it* it)
{
    IDS_CList_NBeginIt(IDS_CList_Tail(clist), it);
}

inline void
IDS_CList_ItFwd (struct ids_clist_it* it)
{
    it->current_node = it->current_node->next;
}

inline void
IDS_CList_ItBack (struct ids_clist_it* it)
{
    it->current_node = it->current_node->prev;
}

inline int
IDS_CList_ItDone (struct ids_clist* clist, struct ids_clist_it* it)
{
    return it->current_node == IDS_CList_End(clist);
}
