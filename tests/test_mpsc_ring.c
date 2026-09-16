/*
    libids is licensed under the simplified BSD license:

    Copyright 2026, Andrew Gottemoller
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other materials
    provided with the distribution.

    Neither the name Andrew Gottemoller nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific prior written
    permission.
 */


#include <ids/mpsc_ring.h>

#include <assert.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>

#include "test_utils.h"


#define STRESS_CAPACITY    64
#define PRODUCER_COUNT     4
#define ITEMS_PER_PRODUCER 25000
#define ITEM_COUNT         (PRODUCER_COUNT * ITEMS_PER_PRODUCER)


static void
Test_InitReset (void)
{
    struct ids_mpsc_ring       ring;
    struct ids_mpsc_ring_range range;

    Ids_MpscRing_Init(&ring, 4);
    CHECK(ring.capacity == 4);
    CHECK(Ids_MpscRing_Count(&ring) == 0);
    CHECK(Ids_MpscRing_Space(&ring) == 4);
    CHECK(Ids_MpscRing_Empty(&ring));
    CHECK(!Ids_MpscRing_Full(&ring));

    range = Ids_MpscRing_Reserve(&ring, 2);
    Ids_MpscRing_Commit(&ring, &range);
    CHECK(Ids_MpscRing_Count(&ring) == 2);

    Ids_MpscRing_Reset(&ring);
    CHECK(Ids_MpscRing_Count(&ring) == 0);
    CHECK(Ids_MpscRing_Space(&ring) == 4);
}

static void
Test_ReserveCommit (void)
{
    struct ids_mpsc_ring       ring;
    struct ids_mpsc_ring_range range;

    Ids_MpscRing_Init(&ring, 4);

    range = Ids_MpscRing_Reserve(&ring, 3);
    CHECK(range.start == 0);
    CHECK(range.count == 3);
    CHECK(Ids_MpscRing_Count(&ring) == 0);
    CHECK(Ids_MpscRing_Space(&ring) == 1);

    Ids_MpscRing_Commit(&ring, &range);
    CHECK(Ids_MpscRing_Count(&ring) == 3);

    range = Ids_MpscRing_Reserve(&ring, 3);
    CHECK(range.start == 3);
    CHECK(range.count == 1);
    Ids_MpscRing_Commit(&ring, &range);
    CHECK(Ids_MpscRing_Full(&ring));

    range = Ids_MpscRing_Reserve(&ring, 1);
    CHECK(range.count == 0);
    Ids_MpscRing_Commit(&ring, &range);
}

static void
Test_PeekRelease (void)
{
    struct ids_mpsc_ring       ring;
    struct ids_mpsc_ring_range range;

    Ids_MpscRing_Init(&ring, 4);

    range = Ids_MpscRing_Reserve(&ring, 4);
    Ids_MpscRing_Commit(&ring, &range);

    range = Ids_MpscRing_Peek(&ring, 2);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    CHECK(Ids_MpscRing_Count(&ring) == 4);

    Ids_MpscRing_Release(&ring, &range);
    CHECK(Ids_MpscRing_Count(&ring) == 2);
    CHECK(Ids_MpscRing_Space(&ring) == 2);

    range = Ids_MpscRing_Peek(&ring, 3);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_MpscRing_Release(&ring, &range);
    CHECK(Ids_MpscRing_Empty(&ring));
}

static void
Test_Wrap (void)
{
    struct ids_mpsc_ring       ring;
    struct ids_mpsc_ring_range range;

    Ids_MpscRing_Init(&ring, 4);

    range = Ids_MpscRing_Reserve(&ring, 3);
    Ids_MpscRing_Commit(&ring, &range);
    range = Ids_MpscRing_Peek(&ring, 2);
    Ids_MpscRing_Release(&ring, &range);

    range = Ids_MpscRing_Reserve(&ring, 3);
    CHECK(range.start == 3);
    CHECK(range.count == 1);
    Ids_MpscRing_Commit(&ring, &range);

    range = Ids_MpscRing_Reserve(&ring, 2);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_MpscRing_Commit(&ring, &range);

    range = Ids_MpscRing_Peek(&ring, 4);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_MpscRing_Release(&ring, &range);

    range = Ids_MpscRing_Peek(&ring, 4);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_MpscRing_Release(&ring, &range);
}

struct commit_context
{
    struct ids_mpsc_ring*       ring;
    struct ids_mpsc_ring_range* range;
    atomic_int*                 started;
    atomic_int*                 finished;
};

static int
CommitThread (void* argument)
{
    struct commit_context* context = argument;

    atomic_store_explicit(context->started, 1, memory_order_release);
    Ids_MpscRing_Commit(context->ring, context->range);
    atomic_store_explicit(context->finished, 1, memory_order_release);

    return 0;
}

static void
Test_OrderedCommit (void)
{
    struct ids_mpsc_ring       ring;
    struct ids_mpsc_ring_range first;
    struct ids_mpsc_ring_range second;
    struct ids_mpsc_ring_range peek;
    atomic_int                 started  = 0;
    atomic_int                 finished = 0;
    thrd_t                     thread;

    Ids_MpscRing_Init(&ring, 4);
    first  = Ids_MpscRing_Reserve(&ring, 2);
    second = Ids_MpscRing_Reserve(&ring, 2);

    struct commit_context context = {.ring     = &ring,
                                     .range    = &second,
                                     .started  = &started,
                                     .finished = &finished};
    CHECK(thrd_create(&thread, CommitThread, &context) == thrd_success);
    while(!atomic_load_explicit(&started, memory_order_acquire))
        thrd_yield();

    CHECK(!atomic_load_explicit(&finished, memory_order_acquire));
    CHECK(Ids_MpscRing_Count(&ring) == 0);
    CHECK(Ids_MpscRing_Full(&ring));

    Ids_MpscRing_Commit(&ring, &first);
    CHECK(thrd_join(thread, NULL) == thrd_success);
    CHECK(atomic_load_explicit(&finished, memory_order_acquire));
    CHECK(Ids_MpscRing_Count(&ring) == 4);

    peek = Ids_MpscRing_Peek(&ring, 4);
    CHECK(peek.count == 4);
    Ids_MpscRing_Release(&ring, &peek);
}

struct stress_context
{
    struct ids_mpsc_ring ring;
    size_t               values[STRESS_CAPACITY];
    atomic_uchar         seen[ITEM_COUNT];
    atomic_int           start;
};

struct producer_context
{
    struct stress_context* stress;
    size_t                 first;
};

static int
ProducerThread (void* argument)
{
    struct producer_context* context  = argument;
    size_t                   produced = 0;

    while(!atomic_load_explicit(&context->stress->start, memory_order_acquire))
        thrd_yield();

    while(produced < ITEMS_PER_PRODUCER)
    {
        size_t count = IDS_MIN(7, ITEMS_PER_PRODUCER - produced);
        struct ids_mpsc_ring_range range = Ids_MpscRing_Reserve(&context->stress->ring, count);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
            context->stress->values[range.start + index] = context->first + produced + index;

        Ids_MpscRing_Commit(&context->stress->ring, &range);
        produced += range.count;
    }

    return 0;
}

static int
ConsumerThread (void* argument)
{
    struct stress_context* context  = argument;
    size_t                 consumed = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();

    while(consumed < ITEM_COUNT)
    {
        struct ids_mpsc_ring_range range = Ids_MpscRing_Peek(&context->ring, 5);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
        {
            size_t value = context->values[range.start + index];
            CHECK(value < ITEM_COUNT);
            CHECK(atomic_fetch_add_explicit(&context->seen[value], 1, memory_order_relaxed) == 0);
        }

        Ids_MpscRing_Release(&context->ring, &range);
        consumed += range.count;
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

    Ids_MpscRing_Init(&context.ring, STRESS_CAPACITY);
    atomic_init(&context.start, 0);
    for(size_t index = 0; index < ITEM_COUNT; ++index)
        atomic_init(&context.seen[index], 0);

    CHECK(thrd_create(&consumer, ConsumerThread, &context) == thrd_success);
    for(size_t index = 0; index < PRODUCER_COUNT; ++index)
    {
        producer_contexts[index].stress = &context;
        producer_contexts[index].first  = index * ITEMS_PER_PRODUCER;
        CHECK(thrd_create(&producers[index], ProducerThread, &producer_contexts[index]) ==
               thrd_success);
    }

    atomic_store_explicit(&context.start, 1, memory_order_release);
    for(size_t index = 0; index < PRODUCER_COUNT; ++index)
        CHECK(thrd_join(producers[index], NULL) == thrd_success);
    CHECK(thrd_join(consumer, NULL) == thrd_success);

    for(size_t index = 0; index < ITEM_COUNT; ++index)
        CHECK(atomic_load_explicit(&context.seen[index], memory_order_relaxed) == 1);
    CHECK(Ids_MpscRing_Empty(&context.ring));
    CHECK(Ids_MpscRing_Space(&context.ring) == STRESS_CAPACITY);
}

static void
Test_Alignment (void)
{
    CHECK(alignof(struct ids_mpsc_ring) == IDS_CACHE_LINE_SIZE);
}

int
main (void)
{
    Test_InitReset();
    Test_ReserveCommit();
    Test_PeekRelease();
    Test_Wrap();
    Test_OrderedCommit();
    Test_Concurrent();
    Test_Alignment();

    return EXIT_SUCCESS;
}
