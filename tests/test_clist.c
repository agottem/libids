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


struct item
{
    int                   id;
    struct ids_clist_node node;
};


static inline struct item*
NodeToItem (struct ids_clist_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}


static void
Test_InitEmpty (void)
{
    struct ids_clist list;

    Ids_Clist_Init(&list);

    assert(Ids_Clist_Empty(&list));
    assert(Ids_Clist_Head(&list) == Ids_Clist_End(&list));
    assert(Ids_Clist_Tail(&list) == Ids_Clist_End(&list));
}

static void
Test_PushPop (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushH(&a.node, &list);
    Ids_Clist_PushT(&b.node, &list);

    assert(NodeToItem(Ids_Clist_Head(&list))->id == 1);
    assert(NodeToItem(Ids_Clist_Tail(&list))->id == 2);
    assert(Ids_Clist_PopH(&list) == &a.node);
    assert(Ids_Clist_PopT(&list) == &b.node);
    assert(Ids_Clist_Empty(&list));

    Ids_Clist_PushH(&a.node, &list);
    Ids_Clist_PushH(&b.node, &list);

    assert(Ids_Clist_PopT(&list) == &a.node);
    assert(Ids_Clist_PopT(&list) == &b.node);
    assert(Ids_Clist_Empty(&list));
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
    Ids_Clist_PushT(&a.node, &list);
    Ids_Clist_PushT(&b.node, &list);
    Ids_Clist_PushT(&c.node, &list);

    Ids_Clist_Del(&b.node);

    Ids_Clist_BeginIt(&list, &it);
    assert(NodeToItem(it.current_node)->id == 1);
    Ids_Clist_ItFwd(&it);
    assert(NodeToItem(it.current_node)->id == 3);
    Ids_Clist_ItFwd(&it);
    assert(Ids_Clist_ItDone(&list, &it));

    Ids_Clist_RBeginIt(&list, &it);
    assert(NodeToItem(it.current_node)->id == 3);
    Ids_Clist_ItBack(&it);
    assert(NodeToItem(it.current_node)->id == 1);
    Ids_Clist_ItBack(&it);
    assert(Ids_Clist_ItDone(&list, &it));
}

static void
Test_SpliceSingle (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };

    Ids_Clist_Init(&list);
    Ids_Clist_PushT(&a.node, &list);
    Ids_Clist_PushT(&b.node, &list);
    Ids_Clist_PushT(&c.node, &list);

    Ids_Clist_Splice(&b.node, &b.node, &a.node);

    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 2);
    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 1);
    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 3);
    assert(Ids_Clist_Empty(&list));
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
    Ids_Clist_PushT(&a.node, &list);
    Ids_Clist_PushT(&b.node, &list);
    Ids_Clist_PushT(&c.node, &list);
    Ids_Clist_PushT(&d.node, &list);

    Ids_Clist_Splice(&b.node, &c.node, &a.node);

    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 2);
    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 3);
    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 1);
    assert(NodeToItem(Ids_Clist_PopH(&list))->id == 4);
    assert(Ids_Clist_Empty(&list));
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
