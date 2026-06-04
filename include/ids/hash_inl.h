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


#include <ids/utils.h>

#include <stddef.h>
#include <stdlib.h>


static inline void
IDS_Hash_UpdateBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    int done;

    done = IDS_CList_ItDone(&bkt->node_list, &it->clist_it);
    if(!done)
        it->current_node = IDS_CONT_OF(it->clist_it.current_node, struct ids_hash_node, node);
    else
        it->current_node = NULL;
}

inline void
IDS_Hash_Init (unsigned int count, struct ids_hash_bkt* bkts, struct ids_hash* hash)
{
    hash->bkt_count = count;
    hash->bkts      = bkts;

    while(count-- > 0)
        IDS_CList_Init(&bkts[count].node_list);
}

inline enum ids_error_code
IDS_Hash_Create (unsigned int count, struct ids_hash* hash)
{
    struct ids_hash_bkt* bkts;

    bkts = calloc(count, sizeof(struct ids_hash_bkt));
    if(bkts == NULL)
        return ids_error_memory_alloc;

    IDS_Hash_Init(count, bkts, hash);

    return ids_error_none;
}

inline void
IDS_Hash_Destroy (struct ids_hash* hash)
{
    free(hash->bkts);
}

inline void
IDS_Hash_Reset (struct ids_hash* hash)
{
    struct ids_hash_bkt* bkts;
    unsigned int         count;

    bkts  = hash->bkts;
    count = hash->bkt_count;
    while(count-- > 0)
        IDS_CList_Reset(&bkts[count].node_list);
}

inline struct ids_hash_bkt*
IDS_Hash_Bkt (unsigned int value_hash, struct ids_hash* hash)
{
    return &hash->bkts[hash->bkt_count % value_hash];
}

inline struct ids_hash_node*
IDS_Hash_Find (
               unsigned int          value_hash,
               void*                 value,
               struct ids_hash*      hash,
               ids_hash_cmp_type     cmp,
               void*                 user_data,
               struct ids_hash_bkt** searched_bkt
              )
{
    struct ids_hash_bkt_it it;
    struct ids_hash_bkt*   bkt;

    bkt           = IDS_Hash_Bkt(value_hash, hash);
    *searched_bkt = bkt;

    for(IDS_Hash_BeginBktIt(bkt, &it); !IDS_Hash_BktItDone(&it); IDS_Hash_BktItFwd(bkt, &it))
    {
        int delta;

        delta = (*cmp)(value, it.current_node, user_data);
        if(delta == 0)
            return it.current_node;
    }

    return NULL;
}

inline void
IDS_Hash_Ins (struct ids_hash_node* node, struct ids_hash_bkt* bkt)
{
    IDS_CList_PushH(&node->node, &bkt->node_list);
}

inline void
IDS_Hash_Add (unsigned int value_hash, struct ids_hash_node* node, struct ids_hash* hash)
{
    IDS_Hash_Ins(node, IDS_Hash_Bkt(value_hash, hash));
}

inline void
IDS_Hash_Del (struct ids_hash_node* node)
{
    IDS_CList_Del(&node->node);
}

inline int
IDS_Hash_BktEmpty (struct ids_hash_bkt* bkt)
{
    return IDS_CList_Empty(&bkt->node_list);
}

inline void
IDS_Hash_BeginBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    IDS_CList_BeginIt(&bkt->node_list, &it->clist_it);
    IDS_Hash_UpdateBktIt(bkt, it);
}

inline void
IDS_Hash_BktItFwd (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    IDS_CList_ItFwd(&it->clist_it);
    IDS_Hash_UpdateBktIt(bkt, it);
}

inline int
IDS_Hash_BktItDone (struct ids_hash_bkt_it* it)
{
    return it->current_node == NULL;
}
