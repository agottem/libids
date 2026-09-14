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
Ids_SpmcRing_Init (struct ids_spmc_ring* ring, size_t capacity)
{
    assert(capacity > 0 && "SPMC ring capacity must be greater than zero");

    ring->capacity = capacity;
    atomic_init(&ring->write_cursor,   0);
    atomic_init(&ring->claim_cursor,   0);
    atomic_init(&ring->release_cursor, 0);
}

inline void
Ids_SpmcRing_Reset (struct ids_spmc_ring* ring)
{
    atomic_store_explicit(&ring->write_cursor,   0, memory_order_relaxed);
    atomic_store_explicit(&ring->claim_cursor,   0, memory_order_relaxed);
    atomic_store_explicit(&ring->release_cursor, 0, memory_order_relaxed);
}

inline size_t
Ids_SpmcRing_Count (struct ids_spmc_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);
    size_t claim_cursor = atomic_load_explicit(&ring->claim_cursor, memory_order_relaxed);

    return write_cursor - claim_cursor;
}

inline size_t
Ids_SpmcRing_Space (struct ids_spmc_ring* ring)
{
    size_t write_cursor   = atomic_load_explicit(&ring->write_cursor,   memory_order_relaxed);
    size_t release_cursor = atomic_load_explicit(&ring->release_cursor, memory_order_relaxed);

    return ring->capacity - (write_cursor - release_cursor);
}

inline int
Ids_SpmcRing_Empty (struct ids_spmc_ring* ring)
{
    return Ids_SpmcRing_Count(ring) == 0;
}

inline int
Ids_SpmcRing_Full (struct ids_spmc_ring* ring)
{
    return Ids_SpmcRing_Space(ring) == 0;
}

inline struct ids_spmc_ring_range
Ids_SpmcRing_Reserve (struct ids_spmc_ring* ring, size_t count)
{
    size_t write_cursor   = atomic_load_explicit(&ring->write_cursor,   memory_order_relaxed);
    size_t release_cursor = atomic_load_explicit(&ring->release_cursor, memory_order_acquire);
    size_t used           = write_cursor - release_cursor;
    size_t space          = ring->capacity - used;
    size_t start          = write_cursor % ring->capacity;
    size_t contig         = ring->capacity - start;

    struct ids_spmc_ring_range range = {.start  = start,
                                        .count  = IDS_MIN(IDS_MIN(count, space), contig),
                                        .cursor = write_cursor};
    return range;
}

inline void
Ids_SpmcRing_Commit (struct ids_spmc_ring* ring, struct ids_spmc_ring_range* range)
{
    atomic_store_explicit(&ring->write_cursor, range->cursor + range->count, memory_order_release);
}

inline struct ids_spmc_ring_range
Ids_SpmcRing_Claim (struct ids_spmc_ring* ring, size_t count)
{
    size_t start;
    size_t claimed;
    size_t next_cursor;
    size_t claim_cursor = atomic_load_explicit(&ring->claim_cursor, memory_order_relaxed);

    do
    {
        start               = claim_cursor % ring->capacity;
        size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_acquire);
        size_t avail        = write_cursor - claim_cursor;
        size_t contig       = ring->capacity - start;
        claimed             = IDS_MIN(IDS_MIN(count, avail), contig);
        if(claimed == 0)
        {
            struct ids_spmc_ring_range range = {.start = start, .count = 0, .cursor = claim_cursor};
            return range;
        }

        next_cursor = claim_cursor + claimed;
    } while(!atomic_compare_exchange_weak_explicit(&ring->claim_cursor,
                                                   &claim_cursor,
                                                   next_cursor,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed));

    struct ids_spmc_ring_range range = {.start = start, .count = claimed, .cursor = claim_cursor};
    return range;
}

inline void
Ids_SpmcRing_Release (struct ids_spmc_ring* ring, struct ids_spmc_ring_range* range)
{
    if(range->count == 0)
        return;

    while(atomic_load_explicit(&ring->release_cursor, memory_order_acquire) != range->cursor);

    atomic_store_explicit(&ring->release_cursor,
                          range->cursor + range->count,
                          memory_order_release);
}
