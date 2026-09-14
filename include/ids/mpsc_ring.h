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
    The mpsc ring data struct manages multiple-producer-single-consumer ring cursors
 */


#ifndef IDS_MPSC_RING_H
#define IDS_MPSC_RING_H


#include <stdalign.h>
#include <stdatomic.h>
#include <stddef.h>

#include <ids/sys.h>


/*
    The result of reserving or peeking a contiguous range in the ring

    Accessible members:
        start -- the first reserved or peeked index
        count -- the number of reserved or peeked indexes
 */
struct ids_mpsc_ring_range
{
    size_t start;
    size_t count;

    size_t cursor;
};

/*
    The mpsc ring data struct itself

    Accessible members:
        capacity -- the number of indexes managed by the ring
 */
struct ids_mpsc_ring
{
    size_t capacity;

    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t reserve_cursor;
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t commit_cursor;
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t read_cursor;
};


/*
    Initialize an mpsc ring.  The data struct will be empty upon initialization.  This
    function requires exclusive access to the ring
 */
inline void
Ids_MpscRing_Init (struct ids_mpsc_ring* ring, size_t capacity);

/*
    Reset an mpsc ring to the initialized state.  This function requires exclusive access
    to the ring
 */
inline void
Ids_MpscRing_Reset (struct ids_mpsc_ring* ring);


/*
    Return a snapshot of the number of committed indexes available to the consumer
 */
inline size_t
Ids_MpscRing_Count (struct ids_mpsc_ring* ring);

/*
    Return a snapshot of the number of indexes available to reserve
 */
inline size_t
Ids_MpscRing_Space (struct ids_mpsc_ring* ring);

/*
    Return non-zero if no committed indexes are available to the consumer
 */
inline int
Ids_MpscRing_Empty (struct ids_mpsc_ring* ring);

/*
    Return non-zero if no indexes are available to reserve
 */
inline int
Ids_MpscRing_Full (struct ids_mpsc_ring* ring);


/*
    Atomically reserve up to count contiguous indexes for one producer.  The returned count
    may be less than requested if the available producer range wraps or if the ring has less
    free space available.  If no space is available, the returned count is zero.  Every
    nonempty reservation must be committed exactly once
 */
inline struct ids_mpsc_ring_range
Ids_MpscRing_Reserve (struct ids_mpsc_ring* ring, size_t count);

/*
    Commit previously reserved indexes to the consumer.  Reservations are committed in the
    order they were made, so this function may block until earlier producers commit their
    reservations.  Reservations should be short lived.  Producers must not access a range
    after committing it
 */
inline void
Ids_MpscRing_Commit (struct ids_mpsc_ring* ring, struct ids_mpsc_ring_range* range);


/*
    Return up to count contiguous indexes available to the single consumer.  The returned
    count may be less than requested if the available consumer range wraps or if fewer indexes
    are available.  If the ring is empty, the returned count is zero
 */
inline struct ids_mpsc_ring_range
Ids_MpscRing_Peek (struct ids_mpsc_ring* ring, size_t count);

/*
    Release previously peeked indexes back to the producers.  Only the consumer may call this
    function
 */
inline void
Ids_MpscRing_Release (struct ids_mpsc_ring* ring, struct ids_mpsc_ring_range* range);


#include <ids/mpsc_ring_inl.h>


#endif
