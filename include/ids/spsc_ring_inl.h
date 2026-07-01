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


#include <ids/utils.h>

#include <assert.h>


inline void
Ids_SpscRing_Init (size_t capacity, struct ids_spsc_ring* ring)
{
    assert(capacity > 0 && "SPSC ring capacity must be greater than zero");

    ring->capacity = capacity;
    atomic_init(&ring->write_cursor, 0);
    atomic_init(&ring->read_cursor,  0);
}

inline void
Ids_SpscRing_Reset (struct ids_spsc_ring* ring)
{
    atomic_store_explicit(&ring->write_cursor, 0, memory_order_relaxed);
    atomic_store_explicit(&ring->read_cursor,  0, memory_order_relaxed);
}

inline size_t
Ids_SpscRing_Capacity (struct ids_spsc_ring* ring)
{
    return ring->capacity;
}

inline size_t
Ids_SpscRing_Count (struct ids_spsc_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);
    size_t read_cursor  = atomic_load_explicit(&ring->read_cursor,  memory_order_relaxed);

    return write_cursor - read_cursor;
}

inline size_t
Ids_SpscRing_Space (struct ids_spsc_ring* ring)
{
    return ring->capacity - Ids_SpscRing_Count(ring);
}

inline int
Ids_SpscRing_Empty (struct ids_spsc_ring* ring)
{
    return Ids_SpscRing_Count(ring) == 0;
}

inline int
Ids_SpscRing_Full (struct ids_spsc_ring* ring)
{
    return Ids_SpscRing_Count(ring) == ring->capacity;
}

inline struct ids_spsc_ring_range
Ids_SpscRing_Reserve (size_t count, struct ids_spsc_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);
    size_t read_cursor  = atomic_load_explicit(&ring->read_cursor,  memory_order_acquire);
    size_t used         = write_cursor - read_cursor;
    size_t space        = ring->capacity - used;
    size_t start        = write_cursor % ring->capacity;
    size_t contig       = ring->capacity - start;

    struct ids_spsc_ring_range range = {.start = start,
                                        .count = IDS_MIN(IDS_MIN(count, space), contig)};
    return range;
}

inline void
Ids_SpscRing_Commit (struct ids_spsc_ring_range* range, struct ids_spsc_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);

    atomic_store_explicit(&ring->write_cursor, write_cursor + range->count, memory_order_release);
}

inline struct ids_spsc_ring_range
Ids_SpscRing_Peek (size_t count, struct ids_spsc_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_acquire);
    size_t read_cursor  = atomic_load_explicit(&ring->read_cursor,  memory_order_relaxed);
    size_t avail        = write_cursor - read_cursor;
    size_t start        = read_cursor % ring->capacity;
    size_t contig       = ring->capacity - start;

    struct ids_spsc_ring_range range = {.start = start,
                                        .count = IDS_MIN(IDS_MIN(count, avail), contig)};
    return range;
}

inline void
Ids_SpscRing_Release (struct ids_spsc_ring_range* range, struct ids_spsc_ring* ring)
{
    size_t read_cursor = atomic_load_explicit(&ring->read_cursor, memory_order_relaxed);

    atomic_store_explicit(&ring->read_cursor, read_cursor + range->count, memory_order_release);
}
