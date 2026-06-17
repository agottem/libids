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


#include <ids/hash.h>


extern void
Ids_Hash_UpdateBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it);

extern void
Ids_Hash_UpdateIt (struct ids_hash* hash, struct ids_hash_it* it);


extern void
Ids_Hash_Init (unsigned int count, struct ids_hash_bkt* bkts, struct ids_hash* hash);

extern enum ids_error_code
Ids_Hash_Create (unsigned int count, struct ids_hash* hash);

extern void
Ids_Hash_Destroy (struct ids_hash* hash);

extern void
Ids_Hash_Reset (struct ids_hash* hash);


extern struct ids_hash_bkt*
Ids_Hash_Bkt (unsigned int value_hash, struct ids_hash* hash);


extern struct ids_hash_node*
Ids_Hash_Find (unsigned int          value_hash,
               void*                 value,
               struct ids_hash*      hash,
               ids_hash_cmp_t        cmp,
               void*                 user_data,
               struct ids_hash_bkt** searched_bkt);


extern void
Ids_Hash_Ins (struct ids_hash_node* node, struct ids_hash_bkt* bkt);

extern void
Ids_Hash_Add (unsigned int value_hash, struct ids_hash_node* node, struct ids_hash* hash);

extern void
Ids_Hash_Del (struct ids_hash_node* node);

extern int
Ids_Hash_BktEmpty (struct ids_hash_bkt* bkt);


extern void
Ids_Hash_BeginBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it);

extern void
Ids_Hash_BktItFwd (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it);

extern int
Ids_Hash_BktItDone (struct ids_hash_bkt_it* it);


extern void
Ids_Hash_BeginIt (struct ids_hash* hash, struct ids_hash_it* it);

extern void
Ids_Hash_ItFwd (struct ids_hash* hash, struct ids_hash_it* it);

extern int
Ids_Hash_ItDone (struct ids_hash_it* it);
