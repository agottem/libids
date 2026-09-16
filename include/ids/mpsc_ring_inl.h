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


#include <ids/alg.h>

#include <assert.h>


inline void
Ids_MpscRing_Init (struct ids_mpsc_ring* ring, size_t capacity)
{
    assert(capacity > 0 && "MPSC ring capacity must be greater than zero");

    ring->capacity = capacity;
    atomic_init(&ring->reserve_cursor, 0);
    atomic_init(&ring->commit_cursor,  0);
    atomic_init(&ring->read_cursor,    0);
}

inline void
Ids_MpscRing_Reset (struct ids_mpsc_ring* ring)
{
    atomic_store_explicit(&ring->reserve_cursor, 0, memory_order_relaxed);
    atomic_store_explicit(&ring->commit_cursor,  0, memory_order_relaxed);
    atomic_store_explicit(&ring->read_cursor,    0, memory_order_relaxed);
}

inline size_t
Ids_MpscRing_Count (const struct ids_mpsc_ring* ring)
{
    size_t commit_cursor = atomic_load_explicit(&ring->commit_cursor, memory_order_relaxed);
    size_t read_cursor   = atomic_load_explicit(&ring->read_cursor,   memory_order_relaxed);

    return commit_cursor - read_cursor;
}

inline size_t
Ids_MpscRing_Space (const struct ids_mpsc_ring* ring)
{
    size_t reserve_cursor = atomic_load_explicit(&ring->reserve_cursor, memory_order_relaxed);
    size_t read_cursor    = atomic_load_explicit(&ring->read_cursor,    memory_order_relaxed);

    return ring->capacity - (reserve_cursor - read_cursor);
}

inline int
Ids_MpscRing_Empty (const struct ids_mpsc_ring* ring)
{
    return Ids_MpscRing_Count(ring) == 0;
}

inline int
Ids_MpscRing_Full (const struct ids_mpsc_ring* ring)
{
    return Ids_MpscRing_Space(ring) == 0;
}

inline struct ids_mpsc_ring_range
Ids_MpscRing_Reserve (struct ids_mpsc_ring* ring, size_t count)
{
    size_t start;
    size_t reserved;
    size_t next_cursor;
    size_t reserve_cursor = atomic_load_explicit(&ring->reserve_cursor, memory_order_relaxed);

    do
    {
        start              = reserve_cursor % ring->capacity;
        size_t read_cursor = atomic_load_explicit(&ring->read_cursor, memory_order_acquire);
        size_t used        = reserve_cursor - read_cursor;
        size_t space       = ring->capacity - used;
        size_t contig      = ring->capacity - start;
        reserved           = IDS_MIN(IDS_MIN(count, space), contig);
        if(reserved == 0)
        {
            struct ids_mpsc_ring_range range = {.start  = start,
                                                .count  = 0,
                                                .cursor = reserve_cursor};
            return range;
        }

        next_cursor = reserve_cursor + reserved;
    } while(!atomic_compare_exchange_weak_explicit(&ring->reserve_cursor,
                                                   &reserve_cursor,
                                                   next_cursor,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed));

    struct ids_mpsc_ring_range range = {.start  = start,
                                        .count  = reserved,
                                        .cursor = reserve_cursor};
    return range;
}

inline void
Ids_MpscRing_Commit (struct ids_mpsc_ring* ring, const struct ids_mpsc_ring_range* range)
{
    if(range->count == 0)
        return;

    while(atomic_load_explicit(&ring->commit_cursor, memory_order_acquire) != range->cursor);

    atomic_store_explicit(&ring->commit_cursor,
                          range->cursor + range->count,
                          memory_order_release);
}

inline struct ids_mpsc_ring_range
Ids_MpscRing_Peek (const struct ids_mpsc_ring* ring, size_t count)
{
    size_t commit_cursor = atomic_load_explicit(&ring->commit_cursor, memory_order_acquire);
    size_t read_cursor   = atomic_load_explicit(&ring->read_cursor,   memory_order_relaxed);
    size_t avail         = commit_cursor - read_cursor;
    size_t start         = read_cursor % ring->capacity;
    size_t contig        = ring->capacity - start;

    struct ids_mpsc_ring_range range = {.start  = start,
                                        .count  = IDS_MIN(IDS_MIN(count, avail), contig),
                                        .cursor = read_cursor};
    return range;
}

inline void
Ids_MpscRing_Release (struct ids_mpsc_ring* ring, const struct ids_mpsc_ring_range* range)
{
    atomic_store_explicit(&ring->read_cursor,
                          range->cursor + range->count,
                          memory_order_release);
}
