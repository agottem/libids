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


#include <ids/cont_of.h>
#include <ids/slist.h>

#include <assert.h>
#include <stdlib.h>


struct item
{
    int                   id;
    struct ids_slist_node node;
};


static inline struct item*
NodeToItem (struct ids_slist_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}


static void
Test_InitReset (void)
{
    struct ids_slist list;
    struct item      item = {.id = 1};

    Ids_Slist_Init(&list);

    assert(Ids_Slist_Empty(&list));
    assert(Ids_Slist_Head(&list) == NULL);
    assert(Ids_Slist_Tail(&list) == NULL);

    Ids_Slist_PushH(&item.node, &list);
    Ids_Slist_Reset(&list);

    assert(Ids_Slist_Empty(&list));
    assert(Ids_Slist_Head(&list) == NULL);
    assert(Ids_Slist_Tail(&list) == NULL);
}

static void
Test_PushPop (void)
{
    struct ids_slist list;
    struct item      a = {.id = 1};
    struct item      b = {.id = 2};
    struct item      c = {.id = 3};

    Ids_Slist_Init(&list);
    Ids_Slist_PushH(&b.node, &list);
    Ids_Slist_PushH(&a.node, &list);
    Ids_Slist_PushT(&c.node, &list);

    assert(NodeToItem(Ids_Slist_Head(&list))->id == 1);
    assert(NodeToItem(Ids_Slist_Tail(&list))->id == 3);
    assert(NodeToItem(Ids_Slist_PopH(&list))->id == 1);
    assert(NodeToItem(Ids_Slist_PopH(&list))->id == 2);
    assert(Ids_Slist_Head(&list) == &c.node);
    assert(Ids_Slist_Tail(&list) == &c.node);
    assert(Ids_Slist_PopH(&list) == &c.node);
    assert(Ids_Slist_Empty(&list));
    assert(Ids_Slist_Tail(&list) == NULL);
}

static void
Test_Ins (void)
{
    struct ids_slist list;
    struct item      a = {.id = 1};
    struct item      b = {.id = 2};
    struct item      c = {.id = 3};
    struct item      d = {.id = 4};

    Ids_Slist_Init(&list);
    Ids_Slist_PushT(&a.node, &list);
    Ids_Slist_PushT(&d.node, &list);
    Ids_Slist_Ins(&b.node, &a.node, &list);
    Ids_Slist_Ins(&c.node, &b.node, &list);

    assert(NodeToItem(Ids_Slist_PopH(&list))->id == 1);
    assert(NodeToItem(Ids_Slist_PopH(&list))->id == 2);
    assert(NodeToItem(Ids_Slist_PopH(&list))->id == 3);
    assert(Ids_Slist_Head(&list) == &d.node);
    assert(Ids_Slist_Tail(&list) == &d.node);

    Ids_Slist_Ins(&a.node, &d.node, &list);
    assert(Ids_Slist_Tail(&list) == &a.node);
    assert(d.node.next == &a.node);
    assert(a.node.next == NULL);
}

static void
Test_It (void)
{
    struct ids_slist    list;
    struct ids_slist_it it;
    struct item         a = {.id = 1};
    struct item         b = {.id = 2};
    struct item         c = {.id = 3};

    Ids_Slist_Init(&list);
    Ids_Slist_BeginIt(&list, &it);
    assert(Ids_Slist_ItDone(&it));

    Ids_Slist_PushT(&a.node, &list);
    Ids_Slist_PushT(&b.node, &list);
    Ids_Slist_PushT(&c.node, &list);

    Ids_Slist_BeginIt(&list, &it);
    assert(NodeToItem(it.node)->id == 1);
    Ids_Slist_ItFwd(&it);
    assert(NodeToItem(it.node)->id == 2);
    Ids_Slist_ItFwd(&it);
    assert(NodeToItem(it.node)->id == 3);
    Ids_Slist_ItFwd(&it);
    assert(Ids_Slist_ItDone(&it));

    Ids_Slist_NBeginIt(&b.node, &it);
    assert(NodeToItem(it.node)->id == 2);
    Ids_Slist_ItFwd(&it);
    assert(NodeToItem(it.node)->id == 3);
}

static void
Test_Reuse (void)
{
    struct ids_slist list;
    struct item      item = {.id = 1};

    Ids_Slist_Init(&list);
    Ids_Slist_PushH(&item.node, &list);
    assert(Ids_Slist_PopH(&list) == &item.node);

    Ids_Slist_PushT(&item.node, &list);
    assert(Ids_Slist_Head(&list) == &item.node);
    assert(Ids_Slist_Tail(&list) == &item.node);
    assert(item.node.next == NULL);
}

int
main (void)
{
    Test_InitReset();
    Test_PushPop();
    Test_Ins();
    Test_It();
    Test_Reuse();

    return EXIT_SUCCESS;
}
