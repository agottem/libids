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
#include <ids/utils.h>

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

    IDS_CList_Init(&list);

    assert(IDS_CList_Empty(&list));
    assert(IDS_CList_Head(&list) == IDS_CList_End(&list));
    assert(IDS_CList_Tail(&list) == IDS_CList_End(&list));
}

static void
Test_PushPop (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };

    IDS_CList_Init(&list);
    IDS_CList_PushH(&a.node, &list);
    IDS_CList_PushT(&b.node, &list);

    assert(NodeToItem(IDS_CList_Head(&list))->id == 1);
    assert(NodeToItem(IDS_CList_Tail(&list))->id == 2);
    assert(IDS_CList_PopH(&list) == &a.node);
    assert(IDS_CList_PopT(&list) == &b.node);
    assert(IDS_CList_Empty(&list));

    IDS_CList_PushH(&a.node, &list);
    IDS_CList_PushH(&b.node, &list);

    assert(IDS_CList_PopT(&list) == &a.node);
    assert(IDS_CList_PopT(&list) == &b.node);
    assert(IDS_CList_Empty(&list));
}

static void
Test_ItAndDel (void)
{
    struct ids_clist    list;
    struct ids_clist_it it;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };

    IDS_CList_Init(&list);
    IDS_CList_PushT(&a.node, &list);
    IDS_CList_PushT(&b.node, &list);
    IDS_CList_PushT(&c.node, &list);

    IDS_CList_Del(&b.node);

    IDS_CList_BeginIt(&list, &it);
    assert(NodeToItem(it.current_node)->id == 1);
    IDS_CList_ItFwd(&it);
    assert(NodeToItem(it.current_node)->id == 3);
    IDS_CList_ItFwd(&it);
    assert(IDS_CList_ItDone(&list, &it));

    IDS_CList_RBeginIt(&list, &it);
    assert(NodeToItem(it.current_node)->id == 3);
    IDS_CList_ItBack(&it);
    assert(NodeToItem(it.current_node)->id == 1);
    IDS_CList_ItBack(&it);
    assert(IDS_CList_ItDone(&list, &it));
}

static void
Test_SpliceSingle (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };

    IDS_CList_Init(&list);
    IDS_CList_PushT(&a.node, &list);
    IDS_CList_PushT(&b.node, &list);
    IDS_CList_PushT(&c.node, &list);

    IDS_CList_Splice(&b.node, &b.node, &a.node);

    assert(NodeToItem(IDS_CList_PopH(&list))->id == 2);
    assert(NodeToItem(IDS_CList_PopH(&list))->id == 1);
    assert(NodeToItem(IDS_CList_PopH(&list))->id == 3);
    assert(IDS_CList_Empty(&list));
}

static void
Test_SpliceRange (void)
{
    struct ids_clist list;

    struct item a = { .id = 1 };
    struct item b = { .id = 2 };
    struct item c = { .id = 3 };
    struct item d = { .id = 4 };

    IDS_CList_Init(&list);
    IDS_CList_PushT(&a.node, &list);
    IDS_CList_PushT(&b.node, &list);
    IDS_CList_PushT(&c.node, &list);
    IDS_CList_PushT(&d.node, &list);

    IDS_CList_Splice(&b.node, &c.node, &a.node);

    assert(NodeToItem(IDS_CList_PopH(&list))->id == 2);
    assert(NodeToItem(IDS_CList_PopH(&list))->id == 3);
    assert(NodeToItem(IDS_CList_PopH(&list))->id == 1);
    assert(NodeToItem(IDS_CList_PopH(&list))->id == 4);
    assert(IDS_CList_Empty(&list));
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
