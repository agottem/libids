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


#include <ids/clist.h>


extern void
IDS_CList_Init (struct ids_clist* clist);

extern void
IDS_CList_Reset (struct ids_clist* clist);


extern struct ids_clist_node*
IDS_CList_Head (struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_Tail (struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_End (struct ids_clist* clist);

extern int
IDS_CList_Empty (struct ids_clist* clist);


extern void
IDS_CList_Splice (struct ids_clist_node* start,
                  struct ids_clist_node* end,
                  struct ids_clist_node* dest);

extern void
IDS_CList_Ins (struct ids_clist_node* restrict new_node,
               struct ids_clist_node* restrict existing_node);

extern void
IDS_CList_Del (struct ids_clist_node* node);

extern void
IDS_CList_PushH (struct ids_clist_node* node, struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_PopH (struct ids_clist* clist);

extern void
IDS_CList_PushT (struct ids_clist_node* node, struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_PopT (struct ids_clist* clist);


extern void
IDS_CList_NBeginIt (struct ids_clist_node* node, struct ids_clist_it* it);

extern void
IDS_CList_BeginIt (struct ids_clist* clist, struct ids_clist_it* it);

extern void
IDS_CList_RBeginIt (struct ids_clist* clist, struct ids_clist_it* it);

extern void
IDS_CList_ItFwd (struct ids_clist_it* it);

extern void
IDS_CList_ItBack (struct ids_clist_it* it);

extern int
IDS_CList_ItDone (struct ids_clist* clist, struct ids_clist_it* it);
