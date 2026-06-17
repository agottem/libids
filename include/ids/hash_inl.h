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

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>


inline void
Ids_Hash_UpdateBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    int done = Ids_Clist_ItDone(&bkt->node_list, &it->clist_it);
    if(!done)
        it->current_node = IDS_CONT_OF(it->clist_it.current_node, struct ids_hash_node, node);
    else
        it->current_node = NULL;
}

inline void
Ids_Hash_UpdateIt (struct ids_hash* hash, struct ids_hash_it* it)
{
    while(Ids_Hash_BktItDone(&it->bkt_it) && it->current_bkt < hash->bkt_count)
    {
        it->current_bkt++;
        if(it->current_bkt >= hash->bkt_count)
        {
            it->current_node = NULL;
            return;
        }

        Ids_Hash_BeginBktIt(&hash->bkts[it->current_bkt], &it->bkt_it);
    }

    it->current_node = it->bkt_it.current_node;
}

inline void
Ids_Hash_Init (unsigned int count, struct ids_hash_bkt* bkts, struct ids_hash* hash)
{
    assert(count > 0 && "Hash bucket count must be greater than zero");

    hash->bkt_count = count;
    hash->bkts      = bkts;

    while(count-- > 0)
        Ids_Clist_Init(&bkts[count].node_list);
}

inline enum ids_error_code
Ids_Hash_Create (unsigned int count, struct ids_hash* hash)
{
    size_t               alloc_size = count * sizeof(struct ids_hash_bkt);
    struct ids_hash_bkt* bkts       = malloc(alloc_size);
    if(bkts == NULL)
        return ids_error_mem;

    Ids_Hash_Init(count, bkts, hash);

    return ids_error_none;
}

inline void
Ids_Hash_Destroy (struct ids_hash* hash)
{
    free(hash->bkts);
}

inline void
Ids_Hash_Reset (struct ids_hash* hash)
{
    struct ids_hash_bkt* bkts  = hash->bkts;
    unsigned int         count = hash->bkt_count;
    while(count-- > 0)
        Ids_Clist_Reset(&bkts[count].node_list);
}

inline struct ids_hash_bkt*
Ids_Hash_Bkt (unsigned int value_hash, struct ids_hash* hash)
{
    return &hash->bkts[value_hash % hash->bkt_count];
}

inline struct ids_hash_node*
Ids_Hash_Find (unsigned int          value_hash,
               void*                 value,
               struct ids_hash*      hash,
               ids_hash_cmp_t*       cmp,
               void*                 user_data,
               struct ids_hash_bkt** searched_bkt)
{
    struct ids_hash_bkt* bkt = Ids_Hash_Bkt(value_hash, hash);
    if(searched_bkt != NULL)
        *searched_bkt = bkt;

    struct ids_hash_bkt_it it;
    for(Ids_Hash_BeginBktIt(bkt, &it); !Ids_Hash_BktItDone(&it); Ids_Hash_BktItFwd(bkt, &it))
    {
        int delta;

        delta = (*cmp)(value, it.current_node, user_data);
        if(delta == 0)
            return it.current_node;
    }

    return NULL;
}

inline void
Ids_Hash_Ins (struct ids_hash_node* node, struct ids_hash_bkt* bkt)
{
    Ids_Clist_PushH(&node->node, &bkt->node_list);
}

inline void
Ids_Hash_Add (unsigned int value_hash, struct ids_hash_node* node, struct ids_hash* hash)
{
    Ids_Hash_Ins(node, Ids_Hash_Bkt(value_hash, hash));
}

inline void
Ids_Hash_Del (struct ids_hash_node* node)
{
    Ids_Clist_Del(&node->node);
}

inline int
Ids_Hash_BktEmpty (struct ids_hash_bkt* bkt)
{
    return Ids_Clist_Empty(&bkt->node_list);
}

inline void
Ids_Hash_BeginBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    Ids_Clist_BeginIt(&bkt->node_list, &it->clist_it);
    Ids_Hash_UpdateBktIt(bkt, it);
}

inline void
Ids_Hash_BktItFwd (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it)
{
    Ids_Clist_ItFwd(&it->clist_it);
    Ids_Hash_UpdateBktIt(bkt, it);
}

inline int
Ids_Hash_BktItDone (struct ids_hash_bkt_it* it)
{
    return it->current_node == NULL;
}

inline void
Ids_Hash_BeginIt (struct ids_hash* hash, struct ids_hash_it* it)
{
    it->current_bkt = 0;
    Ids_Hash_BeginBktIt(&hash->bkts[0], &it->bkt_it);
    Ids_Hash_UpdateIt(hash, it);
}

inline void
Ids_Hash_ItFwd (struct ids_hash* hash, struct ids_hash_it* it)
{
    Ids_Hash_BktItFwd(&hash->bkts[it->current_bkt], &it->bkt_it);
    Ids_Hash_UpdateIt(hash, it);
}

inline int
Ids_Hash_ItDone (struct ids_hash_it* it)
{
    return it->current_node == NULL;
}
