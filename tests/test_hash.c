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
#include <ids/cont_of.h>

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

    struct item         item = {.id = 1};

    Ids_Hash_Init(3, bkts, &hash);
    assert(hash.bkt_count == 3);
    assert(hash.bkts == bkts);
    assert(Ids_Hash_BktEmpty(&bkts[0]));
    assert(Ids_Hash_BktEmpty(&bkts[1]));
    assert(Ids_Hash_BktEmpty(&bkts[2]));

    Ids_Hash_Add(1, &item.node, &hash);
    assert(!Ids_Hash_BktEmpty(Ids_Hash_Bkt(1, &hash)));

    Ids_Hash_Reset(&hash);
    assert(Ids_Hash_BktEmpty(&bkts[0]));
    assert(Ids_Hash_BktEmpty(&bkts[1]));
    assert(Ids_Hash_BktEmpty(&bkts[2]));
}

static void
Test_CreateDestroy (void)
{
    struct ids_hash hash;

    assert(Ids_Hash_Create(4, &hash) == ids_err_none);
    assert(hash.bkt_count == 4);
    assert(hash.bkts != 0);
    Ids_Hash_Destroy(&hash);
}

static void
Test_FindDel (void)
{
    struct ids_hash_bkt  bkts[2];
    struct ids_hash_bkt* searched_bkt;
    struct ids_hash      hash;
    int                  id;

    struct item          a = {.id = 1};
    struct item          b = {.id = 3};

    Ids_Hash_Init(2, bkts, &hash);
    Ids_Hash_Add(1, &a.node, &hash);
    Ids_Hash_Add(3, &b.node, &hash);

    id = 3;
    assert(Ids_Hash_Find(3, &id, &hash, ItemCmp, 0, &searched_bkt) == &b.node);
    assert(searched_bkt == Ids_Hash_Bkt(3, &hash));

    id = 2;
    assert(Ids_Hash_Find(2, &id, &hash, ItemCmp, 0, &searched_bkt) == NULL);

    Ids_Hash_Del(&b.node);
    id = 3;
    assert(Ids_Hash_Find(3, &id, &hash, ItemCmp, 0, &searched_bkt) == NULL);
}

static void
Test_BktIt (void)
{
    struct ids_hash_bkt    bkts[1];
    struct ids_hash_bkt_it it;
    struct ids_hash        hash;

    struct item            a = {.id = 1};
    struct item            b = {.id = 2};

    Ids_Hash_Init(1, bkts, &hash);

    Ids_Hash_BeginBktIt(&bkts[0], &it);
    assert(Ids_Hash_BktItDone(&it));

    Ids_Hash_Add(0, &a.node, &hash);
    Ids_Hash_Add(0, &b.node, &hash);

    Ids_Hash_BeginBktIt(&bkts[0], &it);
    assert(!Ids_Hash_BktItDone(&it));
    assert(it.current_node == &b.node);
    Ids_Hash_BktItFwd(&bkts[0], &it);
    assert(!Ids_Hash_BktItDone(&it));
    assert(it.current_node == &a.node);
    Ids_Hash_BktItFwd(&bkts[0], &it);
    assert(Ids_Hash_BktItDone(&it));
}

static void
Test_It (void)
{
    struct ids_hash_bkt bkts[4];
    struct ids_hash_it  it;
    struct ids_hash     hash;

    int                 visited[5] = { 0 };
    int                 count      = 0;
    struct item         a          = {.id = 1};
    struct item         b          = {.id = 2};
    struct item         c          = {.id = 3};
    struct item         d          = {.id = 4};

    Ids_Hash_Init(4, bkts, &hash);

    Ids_Hash_BeginIt(&hash, &it);
    assert(Ids_Hash_ItDone(&it));

    Ids_Hash_Add(0, &a.node, &hash);
    Ids_Hash_Add(2, &b.node, &hash);
    Ids_Hash_Add(6, &c.node, &hash);
    Ids_Hash_Add(3, &d.node, &hash);

    for(Ids_Hash_BeginIt(&hash, &it); !Ids_Hash_ItDone(&it); Ids_Hash_ItFwd(&hash, &it))
    {
        struct item* item = IDS_CONT_OF(it.current_node, struct item, node);
        assert(item->id >= 1 && item->id <= 4);
        assert(!visited[item->id]);

        visited[item->id] = 1;
        count++;
    }

    assert(count == 4);
    assert(visited[1]);
    assert(visited[2]);
    assert(visited[3]);
    assert(visited[4]);
}

int
main (void)
{
    Test_InitReset();
    Test_CreateDestroy();
    Test_FindDel();
    Test_BktIt();
    Test_It();

    return EXIT_SUCCESS;
}
