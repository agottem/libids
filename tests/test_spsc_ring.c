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


#include <ids/spsc_ring.h>

#include <assert.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>


#define STRESS_CAPACITY 64
#define STRESS_COUNT    100000


static void
Test_InitReset (void)
{
    struct ids_spsc_ring       ring;
    struct ids_spsc_ring_range range;

    Ids_SpscRing_Init(4, &ring);
    assert(ring.capacity == 4);
    assert(Ids_SpscRing_Count(&ring) == 0);
    assert(Ids_SpscRing_Space(&ring) == 4);
    assert(Ids_SpscRing_Empty(&ring));
    assert(!Ids_SpscRing_Full(&ring));

    range = Ids_SpscRing_Reserve(2, &ring);
    Ids_SpscRing_Commit(&range, &ring);
    assert(Ids_SpscRing_Count(&ring) == 2);

    Ids_SpscRing_Reset(&ring);
    assert(Ids_SpscRing_Count(&ring) == 0);
    assert(Ids_SpscRing_Space(&ring) == 4);
}

static void
Test_ReserveCommit (void)
{
    struct ids_spsc_ring       ring;
    struct ids_spsc_ring_range range;

    Ids_SpscRing_Init(4, &ring);

    range = Ids_SpscRing_Reserve(3, &ring);
    assert(range.start == 0);
    assert(range.count == 3);
    assert(Ids_SpscRing_Count(&ring) == 0);

    Ids_SpscRing_Commit(&range, &ring);
    assert(Ids_SpscRing_Count(&ring) == 3);
    assert(Ids_SpscRing_Space(&ring) == 1);

    range = Ids_SpscRing_Reserve(3, &ring);
    assert(range.start == 3);
    assert(range.count == 1);

    Ids_SpscRing_Commit(&range, &ring);
    assert(Ids_SpscRing_Full(&ring));
}

static void
Test_PeekRelease (void)
{
    struct ids_spsc_ring       ring;
    struct ids_spsc_ring_range range;

    Ids_SpscRing_Init(4, &ring);

    range = Ids_SpscRing_Reserve(4, &ring);
    Ids_SpscRing_Commit(&range, &ring);

    range = Ids_SpscRing_Peek(2, &ring);
    assert(range.start == 0);
    assert(range.count == 2);
    assert(Ids_SpscRing_Count(&ring) == 4);

    Ids_SpscRing_Release(&range, &ring);
    assert(Ids_SpscRing_Count(&ring) == 2);
    assert(Ids_SpscRing_Space(&ring) == 2);

    range = Ids_SpscRing_Peek(3, &ring);
    assert(range.start == 2);
    assert(range.count == 2);

    Ids_SpscRing_Release(&range, &ring);
    assert(Ids_SpscRing_Empty(&ring));
}

static void
Test_Wrap (void)
{
    struct ids_spsc_ring       ring;
    struct ids_spsc_ring_range range;

    Ids_SpscRing_Init(4, &ring);

    range = Ids_SpscRing_Reserve(3, &ring);
    Ids_SpscRing_Commit(&range, &ring);
    range = Ids_SpscRing_Peek(2, &ring);
    Ids_SpscRing_Release(&range, &ring);

    range = Ids_SpscRing_Reserve(3, &ring);
    assert(range.start == 3);
    assert(range.count == 1);
    Ids_SpscRing_Commit(&range, &ring);

    range = Ids_SpscRing_Reserve(2, &ring);
    assert(range.start == 0);
    assert(range.count == 2);
    Ids_SpscRing_Commit(&range, &ring);
    assert(Ids_SpscRing_Full(&ring));

    range = Ids_SpscRing_Peek(4, &ring);
    assert(range.start == 2);
    assert(range.count == 2);
    Ids_SpscRing_Release(&range, &ring);

    range = Ids_SpscRing_Peek(4, &ring);
    assert(range.start == 0);
    assert(range.count == 2);
}

static void
Test_Alignment (void)
{
    assert(alignof(struct ids_spsc_ring) == IDS_CACHE_LINE_SIZE);
}

struct stress_context
{
    struct ids_spsc_ring ring;
    size_t                values[STRESS_CAPACITY];
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
        struct ids_spsc_ring_range range = Ids_SpscRing_Reserve(count, &context->ring);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
            context->values[range.start + index] = produced + index;

        Ids_SpscRing_Commit(&range, &context->ring);
        produced += range.count;
    }

    return 0;
}

static int
ConsumerThread (void* argument)
{
    struct stress_context* context = argument;
    size_t                 consumed = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();

    while(consumed < STRESS_COUNT)
    {
        struct ids_spsc_ring_range range = Ids_SpscRing_Peek(5, &context->ring);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }

        for(size_t index = 0; index < range.count; ++index)
            assert(context->values[range.start + index] == consumed + index);

        Ids_SpscRing_Release(&range, &context->ring);
        consumed += range.count;
    }

    return 0;
}

static void
Test_Concurrent (void)
{
    struct stress_context context;
    thrd_t                producer;
    thrd_t                consumer;

    Ids_SpscRing_Init(STRESS_CAPACITY, &context.ring);
    atomic_init(&context.start, 0);

    assert(thrd_create(&producer, ProducerThread, &context) == thrd_success);
    assert(thrd_create(&consumer, ConsumerThread, &context) == thrd_success);
    atomic_store_explicit(&context.start, 1, memory_order_release);

    assert(thrd_join(producer, NULL) == thrd_success);
    assert(thrd_join(consumer, NULL) == thrd_success);
    assert(Ids_SpscRing_Empty(&context.ring));
}

int
main (void)
{
    Test_InitReset();
    Test_ReserveCommit();
    Test_PeekRelease();
    Test_Wrap();
    Test_Alignment();
    Test_Concurrent();

    return EXIT_SUCCESS;
}
