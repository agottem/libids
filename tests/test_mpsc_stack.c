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
#include <ids/mpsc_stack.h>

#include <assert.h>
#include <stdlib.h>


struct item
{
    int                        id;
    struct ids_mpsc_stack_node node;
};


static inline struct item*
NodeToItem (struct ids_mpsc_stack_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}


static void
Test_InitEmpty (void)
{
    struct ids_mpsc_stack stack;

    Ids_MpscStack_Init(&stack);

    assert(Ids_MpscStack_Empty(&stack));
    assert(Ids_MpscStack_Pop(&stack) == NULL);
}

static void
Test_PushPop (void)
{
    struct ids_mpsc_stack stack;
    struct item           a = {.id = 1};
    struct item           b = {.id = 2};
    struct item           c = {.id = 3};

    Ids_MpscStack_Init(&stack);
    Ids_MpscStack_Push(&a.node, &stack);
    Ids_MpscStack_Push(&b.node, &stack);
    Ids_MpscStack_Push(&c.node, &stack);

    assert(!Ids_MpscStack_Empty(&stack));
    assert(NodeToItem(Ids_MpscStack_Pop(&stack))->id == 3);
    assert(NodeToItem(Ids_MpscStack_Pop(&stack))->id == 2);
    assert(NodeToItem(Ids_MpscStack_Pop(&stack))->id == 1);
    assert(Ids_MpscStack_Empty(&stack));
    assert(Ids_MpscStack_Pop(&stack) == NULL);
}

static void
Test_Reuse (void)
{
    struct ids_mpsc_stack stack;
    struct item           item = {.id = 1};

    Ids_MpscStack_Init(&stack);

    Ids_MpscStack_Push(&item.node, &stack);
    assert(Ids_MpscStack_Pop(&stack) == &item.node);

    Ids_MpscStack_Push(&item.node, &stack);
    assert(Ids_MpscStack_Pop(&stack) == &item.node);
    assert(Ids_MpscStack_Empty(&stack));
}

int
main (void)
{
    Test_InitEmpty();
    Test_PushPop();
    Test_Reuse();

    return EXIT_SUCCESS;
}
