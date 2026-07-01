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
#include <stdlib.h>


static void
Test_InitReset (void)
{
    struct ids_spsc_ring       ring;
    struct ids_spsc_ring_range range;

    Ids_SpscRing_Init(4, &ring);
    assert(Ids_SpscRing_Capacity(&ring) == 4);
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

int
main (void)
{
    Test_InitReset();
    Test_ReserveCommit();
    Test_PeekRelease();
    Test_Wrap();
    Test_Alignment();

    return EXIT_SUCCESS;
}
