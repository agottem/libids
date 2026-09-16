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
#include <ids/cont_of.h>

#include <assert.h>
#include <stdlib.h>

#include "test_utils.h"


struct item
{
    int                   id;
    struct ids_clist_node node;
};


static inline struct item*
NodeToItem (const struct ids_clist_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}


static void
Test_InitEmpty (void)
{
    struct ids_clist list;

    Ids_Clist_Init(&list);

    CHECK(Ids_Clist_Empty(&list));
    CHECK(Ids_Clist_Head(&list) == Ids_Clist_End(&list));
    CHECK(Ids_Clist_Tail(&list) == Ids_Clist_End(&list));
}

static void
Test_PushPop (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushH(&list, &a.node);
    Ids_Clist_PushT(&list, &b.node);

    CHECK(NodeToItem(Ids_Clist_Head(&list))->id == 1);
    CHECK(NodeToItem(Ids_Clist_Tail(&list))->id == 2);
    CHECK(Ids_Clist_PopH(&list) == &a.node);
    CHECK(Ids_Clist_PopT(&list) == &b.node);
    CHECK(Ids_Clist_Empty(&list));

    Ids_Clist_PushH(&list, &a.node);
    Ids_Clist_PushH(&list, &b.node);

    CHECK(Ids_Clist_PopT(&list) == &a.node);
    CHECK(Ids_Clist_PopT(&list) == &b.node);
    CHECK(Ids_Clist_Empty(&list));
}

static void
Test_ItAndDel (void)
{
    struct ids_clist    list;
    struct ids_clist_it it;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushT(&list, &a.node);
    Ids_Clist_PushT(&list, &b.node);
    Ids_Clist_PushT(&list, &c.node);

    Ids_Clist_Del(&b.node);

    Ids_Clist_BeginIt(&list, &it);
    CHECK(NodeToItem(it.node)->id == 1);
    Ids_Clist_ItFwd(&it);
    CHECK(NodeToItem(it.node)->id == 3);
    Ids_Clist_ItFwd(&it);
    CHECK(Ids_Clist_ItDone(&list, &it));

    Ids_Clist_RBeginIt(&list, &it);
    CHECK(NodeToItem(it.node)->id == 3);
    Ids_Clist_ItBack(&it);
    CHECK(NodeToItem(it.node)->id == 1);
    Ids_Clist_ItBack(&it);
    CHECK(Ids_Clist_ItDone(&list, &it));
}

static void
Test_SpliceSingle (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushT(&list, &a.node);
    Ids_Clist_PushT(&list, &b.node);
    Ids_Clist_PushT(&list, &c.node);

    Ids_Clist_Splice(&b.node, &b.node, &a.node);

    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 2);
    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 1);
    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 3);
    CHECK(Ids_Clist_Empty(&list));
}

static void
Test_SpliceRange (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };
    struct item d = { .id = 4 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushT(&list, &a.node);
    Ids_Clist_PushT(&list, &b.node);
    Ids_Clist_PushT(&list, &c.node);
    Ids_Clist_PushT(&list, &d.node);

    Ids_Clist_Splice(&b.node, &c.node, &a.node);

    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 2);
    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 3);
    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 1);
    CHECK(NodeToItem(Ids_Clist_PopH(&list))->id == 4);
    CHECK(Ids_Clist_Empty(&list));
}

int
main (void)
{
    Test_InitEmpty();
    Test_PushPop();
    Test_ItAndDel();
    Test_SpliceSingle();
    Test_SpliceRange();

    return EXIT_SUCCESS;
}
