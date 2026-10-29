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


#include <ids/arith.h>

#include <assert.h>


inline void
Ids_Ring_Init (struct ids_ring* ring, size_t capacity)
{
    assert(capacity > 0 && "Ring capacity must be greater than zero");
    assert((capacity & (capacity - 1)) == 0 && "Ring capacity must be a power of two");

    ring->capacity     = capacity;
    ring->write_cursor = 0;
    ring->read_cursor  = 0;
}

inline void
Ids_Ring_Reset (struct ids_ring* ring)
{
    ring->write_cursor = 0;
    ring->read_cursor  = 0;
}

inline size_t
Ids_Ring_Count (const struct ids_ring* ring)
{
    return ring->write_cursor - ring->read_cursor;
}

inline size_t
Ids_Ring_Space (const struct ids_ring* ring)
{
    return ring->capacity - Ids_Ring_Count(ring);
}

inline int
Ids_Ring_Empty (const struct ids_ring* ring)
{
    return Ids_Ring_Count(ring) == 0;
}

inline int
Ids_Ring_Full (const struct ids_ring* ring)
{
    return Ids_Ring_Count(ring) == ring->capacity;
}

inline struct ids_ring_range
Ids_Ring_Reserve (const struct ids_ring* ring, size_t count)
{
    size_t space  = Ids_Ring_Space(ring);
    size_t start  = ring->write_cursor  % ring->capacity;
    size_t contig = ring->capacity - start;

    struct ids_ring_range range = {.start = start,
                                   .count = IDS_MIN(IDS_MIN(count, space), contig)};
    return range;
}

inline void
Ids_Ring_Commit (struct ids_ring* ring, const struct ids_ring_range* range)
{
    ring->write_cursor += range->count;
}

inline struct ids_ring_range
Ids_Ring_Peek (const struct ids_ring* ring, size_t count)
{
    size_t avail  = Ids_Ring_Count(ring);
    size_t start  = ring->read_cursor % ring->capacity;
    size_t contig = ring->capacity - start;

    struct ids_ring_range range = {.start = start,
                                   .count = IDS_MIN(IDS_MIN(count, avail), contig)};
    return range;
}

inline void
Ids_Ring_Release (struct ids_ring* ring, const struct ids_ring_range* range)
{
    ring->read_cursor += range->count;
}
