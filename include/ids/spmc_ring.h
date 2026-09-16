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

/*
    The spmc ring data struct manages single-producer-multiple-consumer ring cursors
 */


#ifndef IDS_SPMC_RING_H
#define IDS_SPMC_RING_H


#include <stdalign.h>
#include <stdatomic.h>
#include <stddef.h>

#include <ids/sys.h>


/*
    The result of reserving or claiming a contiguous range in the ring

    Accessible members:
        start -- the first reserved or claimed index
        count -- the number of reserved or claimed indexes
 */
struct ids_spmc_ring_range
{
    size_t start;
    size_t count;

    size_t cursor;
};

/*
    The spmc ring data struct itself

    Accessible members:
        capacity -- the number of indexes managed by the ring
 */
struct ids_spmc_ring
{
    size_t                                     capacity;

    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t write_cursor;
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t claim_cursor;
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t release_cursor;
};


/*
    Initialize an spmc ring.  The data struct will be empty upon initialization.  This
    function requires exclusive access to the ring
 */
inline void
Ids_SpmcRing_Init (struct ids_spmc_ring* ring, size_t capacity);

/*
    Reset an spmc ring to the initialized state.  This function requires exclusive access
    to the ring
 */
inline void
Ids_SpmcRing_Reset (struct ids_spmc_ring* ring);


/*
    Return a snapshot of the number of committed indexes available to claim
 */
inline size_t
Ids_SpmcRing_Count (const struct ids_spmc_ring* ring);

/*
    Return a snapshot of the number of indexes available to the producer
 */
inline size_t
Ids_SpmcRing_Space (const struct ids_spmc_ring* ring);

/*
    Return non-zero if no committed indexes are available to claim
 */
inline int
Ids_SpmcRing_Empty (const struct ids_spmc_ring* ring);

/*
    Return non-zero if no indexes are available to the producer
 */
inline int
Ids_SpmcRing_Full (const struct ids_spmc_ring* ring);


/*
    Reserve up to count contiguous indexes for the single producer.  The returned count may
    be less than requested if the available producer range wraps or if the ring has less
    free space available.  If no space is available, the returned count is zero
 */
inline struct ids_spmc_ring_range
Ids_SpmcRing_Reserve (const struct ids_spmc_ring* ring, size_t count);

/*
    Commit previously reserved indexes to the consumers.  Only the producer may call this
    function
 */
inline void
Ids_SpmcRing_Commit (struct ids_spmc_ring* ring, const struct ids_spmc_ring_range* range);


/*
    Atomically claim up to count contiguous indexes for one consumer.  The returned count
    may be less than requested if the available consumer range wraps or if fewer indexes
    are available.  If the ring is empty, the returned count is zero.  Every nonempty claim
    must be released exactly once
 */
inline struct ids_spmc_ring_range
Ids_SpmcRing_Claim (struct ids_spmc_ring* ring, size_t count);

/*
    Release previously claimed indexes back to the producer.  Claims must be released in the
    order they were claimed, so this function may block until earlier consumers release their
    claims.  Claims should be short lived.  Consumers must not access a range after releasing it
 */
inline void
Ids_SpmcRing_Release (struct ids_spmc_ring* ring, const struct ids_spmc_ring_range* range);


#include <ids/spmc_ring_inl.h>


#endif
