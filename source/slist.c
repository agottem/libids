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


#include <ids/slist.h>


extern void
Ids_Slist_Init (struct ids_slist* slist);

extern void
Ids_Slist_Reset (struct ids_slist* slist);


extern struct ids_slist_node*
Ids_Slist_Head (struct ids_slist* slist);

extern struct ids_slist_node*
Ids_Slist_Tail (struct ids_slist* slist);

extern int
Ids_Slist_Empty (struct ids_slist* slist);


extern void
Ids_Slist_Ins (struct ids_slist_node* restrict new_node,
               struct ids_slist_node* restrict existing_node,
               struct ids_slist* slist);

extern void
Ids_Slist_PushH (struct ids_slist_node* node, struct ids_slist* slist);

extern struct ids_slist_node*
Ids_Slist_PopH (struct ids_slist* slist);

extern void
Ids_Slist_PushT (struct ids_slist_node* node, struct ids_slist* slist);


extern void
Ids_Slist_NBeginIt (struct ids_slist_node* node, struct ids_slist_it* it);

extern void
Ids_Slist_BeginIt (struct ids_slist* slist, struct ids_slist_it* it);

extern void
Ids_Slist_ItFwd (struct ids_slist_it* it);

extern int
Ids_Slist_ItDone (struct ids_slist_it* it);
