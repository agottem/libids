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


#include <ids/spmc_ring.h>

#include <assert.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>

#include "test_utils.h"


#define STRESS_CAPACITY 64
#define STRESS_COUNT    100000
#define CONSUMER_COUNT  4


static void
Test_InitReset (void)
{
    struct ids_spmc_ring       ring;
    struct ids_spmc_ring_range range;

    Ids_SpmcRing_Init(&ring, 4);
    CHECK(ring.capacity == 4);
    CHECK(Ids_SpmcRing_Count(&ring) == 0);
    CHECK(Ids_SpmcRing_Space(&ring) == 4);
    CHECK(Ids_SpmcRing_Empty(&ring));
    CHECK(!Ids_SpmcRing_Full(&ring));

    range = Ids_SpmcRing_Reserve(&ring, 2);
    Ids_SpmcRing_Commit(&ring, &range);
    CHECK(Ids_SpmcRing_Count(&ring) == 2);

    Ids_SpmcRing_Reset(&ring);
    CHECK(Ids_SpmcRing_Count(&ring) == 0);
    CHECK(Ids_SpmcRing_Space(&ring) == 4);
}

static void
Test_ReserveClaimRelease (void)
{
    struct ids_spmc_ring       ring;
    struct ids_spmc_ring_range range;

    Ids_SpmcRing_Init(&ring, 4);

    range = Ids_SpmcRing_Reserve(&ring, 4);
    CHECK(range.start == 0);
    CHECK(range.count == 4);
    Ids_SpmcRing_Commit(&ring, &range);
    CHECK(Ids_SpmcRing_Full(&ring));

    range = Ids_SpmcRing_Claim(&ring, 2);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    CHECK(Ids_SpmcRing_Count(&ring) == 2);
    CHECK(Ids_SpmcRing_Space(&ring) == 0);

    Ids_SpmcRing_Release(&ring, &range);
    CHECK(Ids_SpmcRing_Space(&ring) == 2);

    range = Ids_SpmcRing_Claim(&ring, 3);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_SpmcRing_Release(&ring, &range);
    CHECK(Ids_SpmcRing_Empty(&ring));
    CHECK(Ids_SpmcRing_Space(&ring) == 4);
}

static void
Test_Wrap (void)
{
    struct ids_spmc_ring       ring;
    struct ids_spmc_ring_range range;

    Ids_SpmcRing_Init(&ring, 4);

    range = Ids_SpmcRing_Reserve(&ring, 3);
    Ids_SpmcRing_Commit(&ring, &range);
    range = Ids_SpmcRing_Claim(&ring, 2);
    Ids_SpmcRing_Release(&ring, &range);

    range = Ids_SpmcRing_Reserve(&ring, 3);
    CHECK(range.start == 3);
    CHECK(range.count == 1);
    Ids_SpmcRing_Commit(&ring, &range);
    range = Ids_SpmcRing_Reserve(&ring, 2);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_SpmcRing_Commit(&ring, &range);

    range = Ids_SpmcRing_Claim(&ring, 4);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_SpmcRing_Release(&ring, &range);
    range = Ids_SpmcRing_Claim(&ring, 4);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_SpmcRing_Release(&ring, &range);
}

struct release_context
{
    struct ids_spmc_ring*       ring;
    struct ids_spmc_ring_range* range;
    atomic_int*                 started;
    atomic_int*                 finished;
};

static int
ReleaseThread (void* argument)
{
    struct release_context* context = argument;

    atomic_store_explicit(context->started, 1, memory_order_release);
    Ids_SpmcRing_Release(context->ring, context->range);
    atomic_store_explicit(context->finished, 1, memory_order_release);

    return 0;
}

static void
Test_OrderedRelease (void)
{
    struct ids_spmc_ring       ring;
    struct ids_spmc_ring_range reserve;
    struct ids_spmc_ring_range first;
    struct ids_spmc_ring_range second;
    atomic_int                 started  = 0;
    atomic_int                 finished = 0;
    thrd_t                     thread;

    Ids_SpmcRing_Init(&ring, 4);
    reserve = Ids_SpmcRing_Reserve(&ring, 4);
    Ids_SpmcRing_Commit(&ring, &reserve);
    first  = Ids_SpmcRing_Claim(&ring, 2);
    second = Ids_SpmcRing_Claim(&ring, 2);

    struct release_context context = {.ring     = &ring,
                                      .range    = &second,
                                      .started  = &started,
                                      .finished = &finished};
    CHECK(thrd_create(&thread, ReleaseThread, &context) == thrd_success);
    while(!atomic_load_explicit(&started, memory_order_acquire))
        thrd_yield();

    CHECK(!atomic_load_explicit(&finished, memory_order_acquire));
    CHECK(Ids_SpmcRing_Space(&ring) == 0);

    Ids_SpmcRing_Release(&ring, &first);
    CHECK(thrd_join(thread, NULL) == thrd_success);
    CHECK(atomic_load_explicit(&finished, memory_order_acquire));
    CHECK(Ids_SpmcRing_Space(&ring) == 4);
}

struct stress_context
{
    struct ids_spmc_ring ring;
    size_t                values[STRESS_CAPACITY];
    atomic_uchar          seen[STRESS_COUNT];
    atomic_size_t         consumed;
    atomic_int            start;
};

static int
ProducerThread (void* argument)
{
    struct stress_context* context = argument;
    size_t                 produced = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();

    while(produced < STRESS_COUNT)
    {
        size_t count = IDS_MIN(7, STRESS_COUNT - produced);
        struct ids_spmc_ring_range range = Ids_SpmcRing_Reserve(&context->ring, count);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
            context->values[range.start + index] = produced + index;

        Ids_SpmcRing_Commit(&context->ring, &range);
        produced += range.count;
    }

    return 0;
}

static int
ConsumerThread (void* argument)
{
    struct stress_context* context = argument;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();

    for(;;)
    {
        struct ids_spmc_ring_range range = Ids_SpmcRing_Claim(&context->ring, 5);
        if(range.count == 0)
        {
            if(atomic_load_explicit(&context->consumed, memory_order_acquire) == STRESS_COUNT)
                break;

            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
        {
            size_t value = context->values[range.start + index];
            CHECK(value < STRESS_COUNT);
            CHECK(atomic_fetch_add_explicit(&context->seen[value], 1, memory_order_relaxed) == 0);
        }

        while(atomic_load_explicit(&context->ring.release_cursor, memory_order_acquire) !=
              range.cursor)
            thrd_yield();
        Ids_SpmcRing_Release(&context->ring, &range);
        atomic_fetch_add_explicit(&context->consumed, range.count, memory_order_release);
    }

    return 0;
}

static void
Test_Concurrent (void)
{
    struct stress_context context;
    thrd_t                producer;
    thrd_t                consumers[CONSUMER_COUNT];

    Ids_SpmcRing_Init(&context.ring, STRESS_CAPACITY);
    atomic_init(&context.consumed, 0);
    atomic_init(&context.start, 0);
    for(size_t index = 0; index < STRESS_COUNT; ++index)
        atomic_init(&context.seen[index], 0);

    CHECK(thrd_create(&producer, ProducerThread, &context) == thrd_success);
    for(size_t index = 0; index < CONSUMER_COUNT; ++index)
        CHECK(thrd_create(&consumers[index], ConsumerThread, &context) == thrd_success);

    atomic_store_explicit(&context.start, 1, memory_order_release);
    CHECK(thrd_join(producer, NULL) == thrd_success);
    for(size_t index = 0; index < CONSUMER_COUNT; ++index)
        CHECK(thrd_join(consumers[index], NULL) == thrd_success);

    for(size_t index = 0; index < STRESS_COUNT; ++index)
        CHECK(atomic_load_explicit(&context.seen[index], memory_order_relaxed) == 1);
    CHECK(Ids_SpmcRing_Empty(&context.ring));
    CHECK(Ids_SpmcRing_Space(&context.ring) == STRESS_CAPACITY);
}

static void
Test_Alignment (void)
{
    CHECK(alignof(struct ids_spmc_ring) == IDS_CACHE_LINE_SIZE);
}

int
main (void)
{
    Test_InitReset();
    Test_ReserveClaimRelease();
    Test_Wrap();
    Test_OrderedRelease();
    Test_Concurrent();
    Test_Alignment();

    return EXIT_SUCCESS;
}
