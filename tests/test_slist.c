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

#include "test_utils.h"


struct item
{
    int                   id;
    struct ids_slist_node node;
};


static inline struct item*
NodeToItem (const struct ids_slist_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}


static void
Test_InitReset (void)
{
    struct ids_slist list;
    struct item      item = {.id = 1};

    Ids_Slist_Init(&list);

    CHECK(Ids_Slist_Empty(&list));
    CHECK(Ids_Slist_Head(&list) == NULL);
    CHECK(Ids_Slist_Tail(&list) == NULL);

    Ids_Slist_PushH(&list, &item.node);
    Ids_Slist_Reset(&list);

    CHECK(Ids_Slist_Empty(&list));
    CHECK(Ids_Slist_Head(&list) == NULL);
    CHECK(Ids_Slist_Tail(&list) == NULL);
}

static void
Test_PushPop (void)
{
    struct ids_slist list;
    struct item      a = {.id = 1};
    struct item      b = {.id = 2};
    struct item      c = {.id = 3};

    Ids_Slist_Init(&list);
    Ids_Slist_PushH(&list, &b.node);
    Ids_Slist_PushH(&list, &a.node);
    Ids_Slist_PushT(&list, &c.node);

    CHECK(NodeToItem(Ids_Slist_Head(&list))->id == 1);
    CHECK(NodeToItem(Ids_Slist_Tail(&list))->id == 3);
    CHECK(NodeToItem(Ids_Slist_PopH(&list))->id == 1);
    CHECK(NodeToItem(Ids_Slist_PopH(&list))->id == 2);
    CHECK(Ids_Slist_Head(&list) == &c.node);
    CHECK(Ids_Slist_Tail(&list) == &c.node);
    CHECK(Ids_Slist_PopH(&list) == &c.node);
    CHECK(Ids_Slist_Empty(&list));
    CHECK(Ids_Slist_Tail(&list) == NULL);
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
    Ids_Slist_PushT(&list, &a.node);
    Ids_Slist_PushT(&list, &d.node);
    Ids_Slist_Ins(&list, &b.node, &a.node);
    Ids_Slist_Ins(&list, &c.node, &b.node);

    CHECK(NodeToItem(Ids_Slist_PopH(&list))->id == 1);
    CHECK(NodeToItem(Ids_Slist_PopH(&list))->id == 2);
    CHECK(NodeToItem(Ids_Slist_PopH(&list))->id == 3);
    CHECK(Ids_Slist_Head(&list) == &d.node);
    CHECK(Ids_Slist_Tail(&list) == &d.node);

    Ids_Slist_Ins(&list, &a.node, &d.node);
    CHECK(Ids_Slist_Tail(&list) == &a.node);
    CHECK(d.node.next == &a.node);
    CHECK(a.node.next == NULL);
}

static void
Test_SpliceH (void)
{
    struct ids_slist source;
    struct ids_slist dest;
    struct item      a = {.id = 1};
    struct item      b = {.id = 2};
    struct item      c = {.id = 3};
    struct item      d = {.id = 4};

    Ids_Slist_Init(&source);
    Ids_Slist_Init(&dest);
    Ids_Slist_PushT(&source, &a.node);
    Ids_Slist_PushT(&source, &b.node);
    Ids_Slist_PushT(&dest, &c.node);
    Ids_Slist_PushT(&dest, &d.node);

    Ids_Slist_SpliceH(&source, &dest);

    CHECK(Ids_Slist_Empty(&source));
    CHECK(Ids_Slist_Tail(&source) == NULL);
    CHECK(Ids_Slist_Head(&dest) == &a.node);
    CHECK(Ids_Slist_Tail(&dest) == &d.node);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 1);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 2);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 3);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 4);
}

static void
Test_SpliceT (void)
{
    struct ids_slist source;
    struct ids_slist dest;
    struct item      a = {.id = 1};
    struct item      b = {.id = 2};
    struct item      c = {.id = 3};
    struct item      d = {.id = 4};

    Ids_Slist_Init(&source);
    Ids_Slist_Init(&dest);
    Ids_Slist_PushT(&source, &a.node);
    Ids_Slist_PushT(&source, &b.node);
    Ids_Slist_PushT(&dest, &c.node);
    Ids_Slist_PushT(&dest, &d.node);

    Ids_Slist_SpliceT(&source, &dest);

    CHECK(Ids_Slist_Empty(&source));
    CHECK(Ids_Slist_Tail(&source) == NULL);
    CHECK(Ids_Slist_Head(&dest) == &c.node);
    CHECK(Ids_Slist_Tail(&dest) == &b.node);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 3);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 4);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 1);
    CHECK(NodeToItem(Ids_Slist_PopH(&dest))->id == 2);
}

static void
Test_SpliceEmpty (void)
{
    struct ids_slist source;
    struct ids_slist dest;
    struct item      a = {.id = 1};

    Ids_Slist_Init(&source);
    Ids_Slist_Init(&dest);
    Ids_Slist_PushT(&dest, &a.node);

    Ids_Slist_SpliceH(&source, &dest);
    CHECK(Ids_Slist_Empty(&source));
    CHECK(Ids_Slist_Head(&dest) == &a.node);
    CHECK(Ids_Slist_Tail(&dest) == &a.node);

    Ids_Slist_SpliceT(&source, &dest);
    CHECK(Ids_Slist_Empty(&source));
    CHECK(Ids_Slist_Head(&dest) == &a.node);
    CHECK(Ids_Slist_Tail(&dest) == &a.node);

    Ids_Slist_SpliceH(&dest, &source);
    CHECK(Ids_Slist_Empty(&dest));
    CHECK(Ids_Slist_Head(&source) == &a.node);
    CHECK(Ids_Slist_Tail(&source) == &a.node);

    Ids_Slist_SpliceT(&source, &dest);
    CHECK(Ids_Slist_Empty(&source));
    CHECK(Ids_Slist_Head(&dest) == &a.node);
    CHECK(Ids_Slist_Tail(&dest) == &a.node);
    CHECK(a.node.next == NULL);
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
    CHECK(Ids_Slist_ItDone(&it));

    Ids_Slist_PushT(&list, &a.node);
    Ids_Slist_PushT(&list, &b.node);
    Ids_Slist_PushT(&list, &c.node);

    Ids_Slist_BeginIt(&list, &it);
    CHECK(NodeToItem(it.node)->id == 1);
    Ids_Slist_ItFwd(&it);
    CHECK(NodeToItem(it.node)->id == 2);
    Ids_Slist_ItFwd(&it);
    CHECK(NodeToItem(it.node)->id == 3);
    Ids_Slist_ItFwd(&it);
    CHECK(Ids_Slist_ItDone(&it));

    Ids_Slist_NBeginIt(&b.node, &it);
    CHECK(NodeToItem(it.node)->id == 2);
    Ids_Slist_ItFwd(&it);
    CHECK(NodeToItem(it.node)->id == 3);
}

static void
Test_Reuse (void)
{
    struct ids_slist list;
    struct item      item = {.id = 1};

    Ids_Slist_Init(&list);
    Ids_Slist_PushH(&list, &item.node);
    CHECK(Ids_Slist_PopH(&list) == &item.node);

    Ids_Slist_PushT(&list, &item.node);
    CHECK(Ids_Slist_Head(&list) == &item.node);
    CHECK(Ids_Slist_Tail(&list) == &item.node);
    CHECK(item.node.next == NULL);
}

int
main (void)
{
    Test_InitReset();
    Test_PushPop();
    Test_Ins();
    Test_SpliceH();
    Test_SpliceT();
    Test_SpliceEmpty();
    Test_It();
    Test_Reuse();

    return EXIT_SUCCESS;
}
