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


#include <ids/ring.h>

#include <stdint.h>

#include "test_utils.h"


static void
Test_InitReset (void)
{
    struct ids_ring ring;

    Ids_Ring_Init(&ring, 4);
    CHECK(ring.capacity == 4);
    CHECK(Ids_Ring_Count(&ring) == 0);
    CHECK(Ids_Ring_Space(&ring) == 4);
    CHECK(Ids_Ring_Empty(&ring));
    CHECK(!Ids_Ring_Full(&ring));

    struct ids_ring_range range = Ids_Ring_Reserve(&ring, 3);
    Ids_Ring_Commit(&ring, &range);
    range = Ids_Ring_Peek(&ring, 2);
    Ids_Ring_Release(&ring, &range);

    Ids_Ring_Reset(&ring);
    CHECK(ring.capacity == 4);
    CHECK(Ids_Ring_Empty(&ring));
    CHECK(Ids_Ring_Space(&ring) == 4);
    range = Ids_Ring_Reserve(&ring, SIZE_MAX);
    CHECK(range.start == 0);
    CHECK(range.count == 4);
}

static void
Test_Ranges (void)
{
    struct ids_ring ring;

    Ids_Ring_Init(&ring, 4);
    struct ids_ring_range range = Ids_Ring_Peek(&ring, 1);
    CHECK(range.count == 0);
    Ids_Ring_Release(&ring, &range);

    range = Ids_Ring_Reserve(&ring, 0);
    CHECK(range.count == 0);
    Ids_Ring_Commit(&ring, &range);
    CHECK(Ids_Ring_Empty(&ring));

    range = Ids_Ring_Reserve(&ring, SIZE_MAX);
    CHECK(range.start == 0);
    CHECK(range.count == 4);
    CHECK(Ids_Ring_Empty(&ring));
    CHECK(Ids_Ring_Space(&ring) == 4);
    struct ids_ring_range repeated = Ids_Ring_Reserve(&ring, SIZE_MAX);
    CHECK(repeated.start == range.start);
    CHECK(repeated.count == range.count);

    range.count = 3;
    Ids_Ring_Commit(&ring, &range);
    CHECK(Ids_Ring_Count(&ring) == 3);
    CHECK(Ids_Ring_Space(&ring) == 1);
    range = Ids_Ring_Reserve(&ring, 2);
    CHECK(range.start == 3);
    CHECK(range.count == 1);
    Ids_Ring_Commit(&ring, &range);
    CHECK(Ids_Ring_Full(&ring));
    range = Ids_Ring_Reserve(&ring, 1);
    CHECK(range.count == 0);
    Ids_Ring_Commit(&ring, &range);
    CHECK(Ids_Ring_Full(&ring));

    range = Ids_Ring_Peek(&ring, 0);
    CHECK(range.count == 0);
    Ids_Ring_Release(&ring, &range);
    CHECK(Ids_Ring_Full(&ring));
    range = Ids_Ring_Peek(&ring, SIZE_MAX);
    CHECK(range.start == 0);
    CHECK(range.count == 4);
    repeated = Ids_Ring_Peek(&ring, SIZE_MAX);
    CHECK(repeated.start == range.start);
    CHECK(repeated.count == range.count);
    CHECK(Ids_Ring_Full(&ring));
    range.count = 2;
    Ids_Ring_Release(&ring, &range);
    CHECK(Ids_Ring_Count(&ring) == 2);
    range = Ids_Ring_Peek(&ring, SIZE_MAX);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_Ring_Release(&ring, &range);
    CHECK(Ids_Ring_Empty(&ring));
}

static void
Test_Wrap (void)
{
    struct ids_ring ring;

    Ids_Ring_Init(&ring, 4);
    struct ids_ring_range range = Ids_Ring_Reserve(&ring, 3);
    Ids_Ring_Commit(&ring, &range);
    range = Ids_Ring_Peek(&ring, 2);
    Ids_Ring_Release(&ring, &range);

    range = Ids_Ring_Reserve(&ring, 3);
    CHECK(range.start == 3);
    CHECK(range.count == 1);
    Ids_Ring_Commit(&ring, &range);
    range = Ids_Ring_Reserve(&ring, 3);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_Ring_Commit(&ring, &range);
    CHECK(Ids_Ring_Full(&ring));

    range = Ids_Ring_Peek(&ring, 4);
    CHECK(range.start == 2);
    CHECK(range.count == 2);
    Ids_Ring_Release(&ring, &range);
    range = Ids_Ring_Peek(&ring, 4);
    CHECK(range.start == 0);
    CHECK(range.count == 2);
    Ids_Ring_Release(&ring, &range);
    CHECK(Ids_Ring_Empty(&ring));
}

static void
Test_Fifo (size_t capacity, size_t cursor)
{
    struct ids_ring ring;
    size_t         storage[64];
    size_t         produced = 0;
    size_t         consumed = 0;
    size_t         state    = 1;

    Ids_Ring_Init(&ring, capacity);

    /* Start an empty ring near cursor overflow without iterating SIZE_MAX times. */
    ring.write_cursor = cursor;
    ring.read_cursor  = cursor;

    while(consumed < 10000)
    {
        state = state * 1664525u + 1013904223u;
        size_t requested = IDS_MIN((state >> 8) % 17, 10000 - produced);
        struct ids_ring_range range = Ids_Ring_Reserve(&ring, requested);
        CHECK(range.count <= requested);
        CHECK(range.count <= Ids_Ring_Space(&ring));
        CHECK(range.start < capacity);
        CHECK(range.count <= capacity - range.start);
        for(size_t index = 0; index < range.count; ++index)
            storage[range.start + index] = produced + index;
        produced += range.count;
        Ids_Ring_Commit(&ring, &range);

        state = state * 1664525u + 1013904223u;
        requested = (state >> 8) % 13;
        range = Ids_Ring_Peek(&ring, requested);
        CHECK(range.count <= requested);
        CHECK(range.count <= produced - consumed);
        CHECK(range.start < capacity);
        CHECK(range.count <= capacity - range.start);
        for(size_t index = 0; index < range.count; ++index)
            CHECK(storage[range.start + index] == consumed + index);
        consumed += range.count;
        Ids_Ring_Release(&ring, &range);

        CHECK(Ids_Ring_Count(&ring) == produced - consumed);
        CHECK(Ids_Ring_Space(&ring) == capacity - (produced - consumed));
        CHECK(!!Ids_Ring_Empty(&ring) == (produced == consumed));
        CHECK(!!Ids_Ring_Full(&ring) == (produced - consumed == capacity));
    }

    CHECK(produced == consumed);
    CHECK(Ids_Ring_Empty(&ring));
    if(cursor != 0)
    {
        CHECK(ring.write_cursor < cursor);
        CHECK(ring.read_cursor < cursor);
    }
}

int
main (void)
{
    Test_InitReset();
    Test_Ranges();
    Test_Wrap();
    for(size_t capacity = 1; capacity <= 64; capacity *= 2)
    {
        Test_Fifo(capacity, 0);
        Test_Fifo(capacity, SIZE_MAX - 2);
    }

    return EXIT_SUCCESS;
}
