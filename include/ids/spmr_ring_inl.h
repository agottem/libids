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
#include <stdatomic.h>
#include <stddef.h>


inline void
Ids_SpmrRing_Init (struct ids_spmr_ring*        ring,
                   size_t                       capacity,
                   struct ids_spmr_ring_reader* readers,
                   size_t                       reader_count)
{
    assert(capacity > 0 && "SPMR ring capacity must be greater than zero");
    assert((capacity & (capacity - 1)) == 0 &&
           "SPMR ring capacity must be a power of two");
    assert(reader_count > 0 && "SPMR ring reader count must be greater than zero");

    ring->capacity     = capacity;
    ring->readers      = readers;
    ring->reader_count = reader_count;
    atomic_init(&ring->write_cursor, 0);

    for(size_t index = reader_count; index-- > 0;)
        atomic_init(&readers[index].read_cursor, 0);
}

inline void
Ids_SpmrRing_Reset (struct ids_spmr_ring* ring)
{
    atomic_store_explicit(&ring->write_cursor, 0, memory_order_relaxed);

    for(size_t index = ring->reader_count; index-- > 0;)
        atomic_store_explicit(&ring->readers[index].read_cursor, 0, memory_order_relaxed);
}

inline size_t
Ids_SpmrRing_Count (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader)
{
    size_t read_cursor  = atomic_load_explicit(&reader->read_cursor, memory_order_relaxed);
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor,  memory_order_relaxed);

    return write_cursor - read_cursor;
}

inline size_t
Ids_SpmrRing_Space (const struct ids_spmr_ring* ring)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);
    size_t used         = 0;

    for(size_t index = ring->reader_count; index-- > 0;)
    {
        size_t read_cursor = atomic_load_explicit(&ring->readers[index].read_cursor,
                                                  memory_order_acquire);
        used               = IDS_MAX(used, write_cursor - read_cursor);
    }

    return ring->capacity - used;
}

inline int
Ids_SpmrRing_Empty (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader)
{
    return Ids_SpmrRing_Count(ring, reader) == 0;
}

inline int
Ids_SpmrRing_Full (const struct ids_spmr_ring* ring)
{
    return Ids_SpmrRing_Space(ring) == 0;
}

inline struct ids_spmr_ring_range
Ids_SpmrRing_Reserve (const struct ids_spmr_ring* ring, size_t count)
{
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_relaxed);
    size_t space        = Ids_SpmrRing_Space(ring);
    size_t start        = write_cursor % ring->capacity;
    size_t contig       = ring->capacity - start;

    struct ids_spmr_ring_range range = {.start  = start,
                                        .count  = IDS_MIN(IDS_MIN(count, space), contig),
                                        .cursor = write_cursor};
    return range;
}

inline void
Ids_SpmrRing_Commit (struct ids_spmr_ring* ring, const struct ids_spmr_ring_range* range)
{
    atomic_store_explicit(&ring->write_cursor,
                          range->cursor + range->count,
                          memory_order_release);
}

inline struct ids_spmr_ring_range
Ids_SpmrRing_Peek (const struct ids_spmr_ring*        ring,
                   const struct ids_spmr_ring_reader* reader,
                   size_t                             count)
{
    size_t read_cursor  = atomic_load_explicit(&reader->read_cursor, memory_order_relaxed);
    size_t write_cursor = atomic_load_explicit(&ring->write_cursor, memory_order_acquire);
    size_t avail        = write_cursor - read_cursor;
    size_t start        = read_cursor % ring->capacity;
    size_t contig       = ring->capacity - start;

    struct ids_spmr_ring_range range = {.start  = start,
                                        .count  = IDS_MIN(IDS_MIN(count, avail), contig),
                                        .cursor = read_cursor};
    return range;
}

inline void
Ids_SpmrRing_Release (struct ids_spmr_ring_reader* reader, const struct ids_spmr_ring_range* range)
{
    atomic_store_explicit(&reader->read_cursor,
                          range->cursor + range->count,
                          memory_order_release);
}
