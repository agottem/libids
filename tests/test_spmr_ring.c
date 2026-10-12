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


#include <ids/spmr_ring.h>

#include <stdint.h>
#include <stdlib.h>
#include <threads.h>

#include "test_utils.h"


#define STRESS_CAPACITY 64
#define STRESS_COUNT    100000
#define READER_COUNT    4


static void
Test_Broadcast (void)
{
    struct ids_spmr_ring        ring;
    struct ids_spmr_ring_reader readers[2];
    struct ids_spmr_ring_range  range;

    Ids_SpmrRing_Init(&ring, 4, readers, 2);
    CHECK(Ids_SpmrRing_Space(&ring) == 4);
    CHECK(!Ids_SpmrRing_Full(&ring));
    range = Ids_SpmrRing_Reserve(&ring, 8);
    CHECK(range.start == 0 && range.count == 4);
    CHECK(Ids_SpmrRing_Empty(&ring, &readers[0]));
    CHECK(Ids_SpmrRing_Peek(&ring, &readers[1], 5).count == 0);
    Ids_SpmrRing_Commit(&ring, &range);
    CHECK(Ids_SpmrRing_Full(&ring));
    range = Ids_SpmrRing_Reserve(&ring, 1);
    CHECK(range.count == 0);

    range = Ids_SpmrRing_Peek(&ring, &readers[0], 5);
    CHECK(range.start == 0 && range.count == 4);
    Ids_SpmrRing_Release(&readers[0], &range);
    CHECK(Ids_SpmrRing_Empty(&ring, &readers[0]));
    CHECK(Ids_SpmrRing_Count(&ring, &readers[1]) == 4);
    CHECK(Ids_SpmrRing_Space(&ring) == 0);

    range = Ids_SpmrRing_Peek(&ring, &readers[1], 2);
    CHECK(range.start == 0 && range.count == 2);
    CHECK(Ids_SpmrRing_Space(&ring) == 0);
    Ids_SpmrRing_Release(&readers[1], &range);
    CHECK(Ids_SpmrRing_Space(&ring) == 2);
    range = Ids_SpmrRing_Reserve(&ring, 5);
    CHECK(range.start == 0 && range.count == 2);
    Ids_SpmrRing_Commit(&ring, &range);
    CHECK(Ids_SpmrRing_Full(&ring));

    range = Ids_SpmrRing_Peek(&ring, &readers[1], 5);
    CHECK(range.start == 2 && range.count == 2);
    Ids_SpmrRing_Release(&readers[1], &range);
    range = Ids_SpmrRing_Peek(&ring, &readers[1], 5);
    CHECK(range.start == 0 && range.count == 2);
    Ids_SpmrRing_Release(&readers[1], &range);
    CHECK(Ids_SpmrRing_Space(&ring) == 2);

    Ids_SpmrRing_Reset(&ring);
    for(size_t index = 0; index < 2; ++index)
        CHECK(Ids_SpmrRing_Empty(&ring, &readers[index]));
    CHECK(Ids_SpmrRing_Space(&ring) == 4);
    range = Ids_SpmrRing_Reserve(&ring, 3);
    Ids_SpmrRing_Commit(&ring, &range);
    for(size_t index = 0; index < 2; ++index)
    {
        range = Ids_SpmrRing_Peek(&ring, &readers[index], 3);
        Ids_SpmrRing_Release(&readers[index], &range);
    }
    range = Ids_SpmrRing_Reserve(&ring, 5);
    CHECK(range.start == 3 && range.count == 1);
}

static void
Test_CursorWrap (void)
{
    struct ids_spmr_ring        ring;
    struct ids_spmr_ring_reader readers[2];
    struct ids_spmr_ring_range  range;

    Ids_SpmrRing_Init(&ring, 4, readers, 2);
    atomic_store_explicit(&ring.write_cursor, SIZE_MAX - 1, memory_order_relaxed);
    for(size_t index = 0; index < 2; ++index)
        atomic_store_explicit(&readers[index].read_cursor,
                              SIZE_MAX - 1,
                              memory_order_relaxed);

    range = Ids_SpmrRing_Reserve(&ring, 4);
    CHECK(range.start == 2 && range.count == 2);
    Ids_SpmrRing_Commit(&ring, &range);
    CHECK(Ids_SpmrRing_Space(&ring) == 2);
    for(size_t index = 0; index < 2; ++index)
    {
        CHECK(Ids_SpmrRing_Count(&ring, &readers[index]) == 2);
        range = Ids_SpmrRing_Peek(&ring, &readers[index], 4);
        CHECK(range.start == 2 && range.count == 2);
        Ids_SpmrRing_Release(&readers[index], &range);
        CHECK(Ids_SpmrRing_Space(&ring) == (index == 0 ? 2 : 4));
    }
    CHECK(Ids_SpmrRing_Space(&ring) == 4);
    range = Ids_SpmrRing_Reserve(&ring, 4);
    CHECK(range.start == 0 && range.count == 4);
    Ids_SpmrRing_Commit(&ring, &range);
    CHECK(Ids_SpmrRing_Full(&ring));
}

static void
Test_SingleIndex (void)
{
    struct ids_spmr_ring        ring;
    struct ids_spmr_ring_reader reader;
    struct ids_spmr_ring_range  range;

    Ids_SpmrRing_Init(&ring, 1, &reader, 1);
    for(size_t index = 0; index < 8; ++index)
    {
        range = Ids_SpmrRing_Reserve(&ring, 2);
        CHECK(range.start == 0 && range.count == 1);
        Ids_SpmrRing_Commit(&ring, &range);
        CHECK(Ids_SpmrRing_Full(&ring));
        range = Ids_SpmrRing_Peek(&ring, &reader, 2);
        CHECK(range.start == 0 && range.count == 1);
        Ids_SpmrRing_Release(&reader, &range);
        CHECK(Ids_SpmrRing_Empty(&ring, &reader));
        CHECK(Ids_SpmrRing_Space(&ring) == 1);
    }
}

struct stress_context
{
    struct ids_spmr_ring        ring;
    struct ids_spmr_ring_reader readers[READER_COUNT];
    size_t                      values[STRESS_CAPACITY];
    atomic_int                  start;
};

struct reader_context
{
    struct stress_context* context;
    size_t                 index;
};

static int
ProducerThread (void* argument)
{
    struct stress_context* context  = argument;
    size_t                 produced = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();
    while(produced < STRESS_COUNT)
    {
        struct ids_spmr_ring_range range = Ids_SpmrRing_Reserve(&context->ring, IDS_MIN(7, STRESS_COUNT - produced));
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }
        for(size_t index = 0; index < range.count; ++index)
            context->values[range.start + index] = produced + index;
        Ids_SpmrRing_Commit(&context->ring, &range);
        produced += range.count;
    }
    return 0;
}

static int
ReaderThread (void* argument)
{
    struct reader_context*       reader_context = argument;
    struct stress_context*       context        = reader_context->context;
    struct ids_spmr_ring_reader* reader         = &context->readers[reader_context->index];
    size_t                       consumed       = 0;

    while(!atomic_load_explicit(&context->start, memory_order_acquire))
        thrd_yield();
    while(consumed < STRESS_COUNT)
    {
        struct ids_spmr_ring_range range = Ids_SpmrRing_Peek(&context->ring,
                                                             reader,
                                                             reader_context->index + 1);
        if(range.count == 0)
        {
            thrd_yield();
            continue;
        }
        for(size_t index = 0; index < range.count; ++index)
            CHECK(context->values[range.start + index] == consumed + index);
        if(reader_context->index == 0) thrd_yield();
        Ids_SpmrRing_Release(reader, &range);
        consumed += range.count;
    }
    return 0;
}

static void
Test_Concurrent (void)
{
    struct stress_context context;
    struct reader_context reader_contexts[READER_COUNT];
    thrd_t                producer;
    thrd_t                readers[READER_COUNT];

    Ids_SpmrRing_Init(&context.ring, STRESS_CAPACITY, context.readers, READER_COUNT);
    atomic_init(&context.start, 0);
    CHECK(thrd_create(&producer, ProducerThread, &context) == thrd_success);
    for(size_t index = 0; index < READER_COUNT; ++index)
    {
        reader_contexts[index] = (struct reader_context){.context = &context, .index = index};
        CHECK(thrd_create(&readers[index], ReaderThread, &reader_contexts[index]) == thrd_success);
    }
    atomic_store_explicit(&context.start, 1, memory_order_release);
    CHECK(thrd_join(producer, NULL) == thrd_success);
    for(size_t index = 0; index < READER_COUNT; ++index)
    {
        CHECK(thrd_join(readers[index], NULL) == thrd_success);
        CHECK(Ids_SpmrRing_Empty(&context.ring, &context.readers[index]));
    }
    CHECK(Ids_SpmrRing_Space(&context.ring) == STRESS_CAPACITY);
}

int
main (void)
{
    Test_Broadcast();
    Test_CursorWrap();
    Test_SingleIndex();
    Test_Concurrent();
    CHECK(alignof(struct ids_spmr_ring) == IDS_CACHE_LINE_SIZE);
    CHECK(alignof(struct ids_spmr_ring_reader) == IDS_CACHE_LINE_SIZE);
    return EXIT_SUCCESS;
}
