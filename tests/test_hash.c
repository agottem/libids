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
#include <ids/utils.h>

#include <assert.h>
#include <stdlib.h>


struct item
{
    int                  id;
    struct ids_hash_node node;
};


static inline int
ItemCmp (void* id, struct ids_hash_node* node, void* user_data)
{
    struct item* item;

    item = IDS_CONT_OF(node, struct item, node);

    return item->id != *(int*)id;
}


static void
Test_InitReset (void)
{
    struct ids_hash_bkt bkts[3];
    struct ids_hash     hash;

    struct item item = { .id = 1 };

    IDS_Hash_Init(3, bkts, &hash);
    assert(hash.bkt_count == 3);
    assert(hash.bkts == bkts);
    assert(IDS_Hash_BktEmpty(&bkts[0]));
    assert(IDS_Hash_BktEmpty(&bkts[1]));
    assert(IDS_Hash_BktEmpty(&bkts[2]));

    IDS_Hash_Add(1, &item.node, &hash);
    assert(!IDS_Hash_BktEmpty(IDS_Hash_Bkt(1, &hash)));

    IDS_Hash_Reset(&hash);
    assert(IDS_Hash_BktEmpty(&bkts[0]));
    assert(IDS_Hash_BktEmpty(&bkts[1]));
    assert(IDS_Hash_BktEmpty(&bkts[2]));
}

static void
Test_CreateDestroy (void)
{
    struct ids_hash hash;

    assert(IDS_Hash_Create(4, &hash) == ids_error_none);
    assert(hash.bkt_count == 4);
    assert(hash.bkts != 0);
    IDS_Hash_Destroy(&hash);
}

static void
Test_FindDel (void)
{
    struct ids_hash_bkt  bkts[2];
    struct ids_hash_bkt* searched_bkt;
    struct ids_hash      hash;
    int                  id;

    struct item a = { .id = 1 };
    struct item b = { .id = 3 };

    IDS_Hash_Init(2, bkts, &hash);
    IDS_Hash_Add(1, &a.node, &hash);
    IDS_Hash_Add(3, &b.node, &hash);

    id = 3;
    assert(IDS_Hash_Find(3, &id, &hash, ItemCmp, 0, &searched_bkt) == &b.node);
    assert(searched_bkt == IDS_Hash_Bkt(3, &hash));

    id = 2;
    assert(IDS_Hash_Find(2, &id, &hash, ItemCmp, 0, &searched_bkt) == NULL);

    IDS_Hash_Del(&b.node);
    id = 3;
    assert(IDS_Hash_Find(3, &id, &hash, ItemCmp, 0, &searched_bkt) == NULL);
}

static void
Test_BktIt (void)
{
    struct ids_hash_bkt    bkts[1];
    struct ids_hash_bkt_it it;
    struct ids_hash        hash;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };

    IDS_Hash_Init(1, bkts, &hash);

    IDS_Hash_BeginBktIt(&bkts[0], &it);
    assert(IDS_Hash_BktItDone(&it));

    IDS_Hash_Add(0, &a.node, &hash);
    IDS_Hash_Add(0, &b.node, &hash);

    IDS_Hash_BeginBktIt(&bkts[0], &it);
    assert(!IDS_Hash_BktItDone(&it));
    assert(it.current_node == &b.node);
    IDS_Hash_BktItFwd(&bkts[0], &it);
    assert(!IDS_Hash_BktItDone(&it));
    assert(it.current_node == &a.node);
    IDS_Hash_BktItFwd(&bkts[0], &it);
    assert(IDS_Hash_BktItDone(&it));
}

int
main (void)
{
    Test_InitReset();
    Test_CreateDestroy();
    Test_FindDel();
    Test_BktIt();

    return EXIT_SUCCESS;
}
