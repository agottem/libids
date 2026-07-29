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
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>


#define PRODUCER_COUNT 4
#define ITEMS_PER_PRODUCER 25000
#define ITEM_COUNT (PRODUCER_COUNT * ITEMS_PER_PRODUCER)


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

struct stress_context
{
    struct ids_mpsc_stack stack;
    struct item           items[ITEM_COUNT];
    atomic_uchar          seen[ITEM_COUNT];
    atomic_int            start;
};

struct producer_context
{
    struct stress_context* stress;
    size_t                 first;
};

static int
ProducerThread (void* argument)
{
    struct producer_context* context = argument;

    while(!atomic_load_explicit(&context->stress->start, memory_order_acquire))
        thrd_yield();

    for(size_t index = context->first; index < context->first + ITEMS_PER_PRODUCER; ++index)
        Ids_MpscStack_Push(&context->stress->items[index].node, &context->stress->stack);

    return 0;
}

static int
ConsumerThread (void* argument)
{
    struct stress_context* context = argument;
    size_t                 consumed = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();

    while(consumed < ITEM_COUNT)
    {
        struct ids_mpsc_stack_node* node = Ids_MpscStack_Pop(&context->stack);
        if(node == NULL)
        {
            thrd_yield();
            continue;
        }

        struct item* item = NodeToItem(node);
        assert(item->id >= 0 && item->id < ITEM_COUNT);
        assert(atomic_fetch_add_explicit(&context->seen[item->id], 1, memory_order_relaxed) == 0);
        ++consumed;
    }

    return 0;
}

static void
Test_Concurrent (void)
{
    struct stress_context   context;
    struct producer_context producer_contexts[PRODUCER_COUNT];
    thrd_t                  producers[PRODUCER_COUNT];
    thrd_t                  consumer;

    Ids_MpscStack_Init(&context.stack);
    atomic_init(&context.start, 0);
    for(size_t index = 0; index < ITEM_COUNT; ++index)
    {
        context.items[index].id = index;
        atomic_init(&context.seen[index], 0);
    }

    assert(thrd_create(&consumer, ConsumerThread, &context) == thrd_success);
    for(size_t index = 0; index < PRODUCER_COUNT; ++index)
    {
        producer_contexts[index].stress = &context;
        producer_contexts[index].first  = index * ITEMS_PER_PRODUCER;
        assert(thrd_create(&producers[index], ProducerThread, &producer_contexts[index]) ==
               thrd_success);
    }

    atomic_store_explicit(&context.start, 1, memory_order_release);
    for(size_t index = 0; index < PRODUCER_COUNT; ++index)
        assert(thrd_join(producers[index], NULL) == thrd_success);
    assert(thrd_join(consumer, NULL) == thrd_success);

    for(size_t index = 0; index < ITEM_COUNT; ++index)
        assert(atomic_load_explicit(&context.seen[index], memory_order_relaxed) == 1);
    assert(Ids_MpscStack_Empty(&context.stack));
}

int
main (void)
{
    Test_InitEmpty();
    Test_PushPop();
    Test_Reuse();
    Test_Concurrent();

    return EXIT_SUCCESS;
}
