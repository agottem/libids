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
Ids_Slist_Head (const struct ids_slist* slist);

extern struct ids_slist_node*
Ids_Slist_Tail (const struct ids_slist* slist);

extern int
Ids_Slist_Empty (const struct ids_slist* slist);


extern void
Ids_Slist_Ins (struct ids_slist*               slist,
               struct ids_slist_node* restrict new_node,
               struct ids_slist_node* restrict existing_node);

extern void
Ids_Slist_SpliceH (struct ids_slist* restrict source, struct ids_slist* restrict dest);

extern void
Ids_Slist_SpliceT (struct ids_slist* restrict source, struct ids_slist* restrict dest);

extern void
Ids_Slist_PushH (struct ids_slist* slist, struct ids_slist_node* node);

extern struct ids_slist_node*
Ids_Slist_PopH (struct ids_slist* slist);

extern void
Ids_Slist_PushT (struct ids_slist* slist, struct ids_slist_node* node);


extern void
Ids_Slist_NBeginIt (const struct ids_slist_node* node, struct ids_slist_it* it);

extern void
Ids_Slist_BeginIt (const struct ids_slist* slist, struct ids_slist_it* it);

extern void
Ids_Slist_ItFwd (struct ids_slist_it* it);

extern int
Ids_Slist_ItDone (const struct ids_slist_it* it);
