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

/*
    The spsc ring data struct manages single-producer-single-consumer ring cursors
 */


#ifndef IDS_SPSC_RING_H
#define IDS_SPSC_RING_H


#include <stdatomic.h>
#include <stdalign.h>
#include <stddef.h>

#include <ids/sys.h>



/*
    The result of reserving a contiguous range in the ring

    Accessible members:
        start -- the first reserved index
        count -- the number of reserved indexes
 */
struct ids_spsc_ring_range
{
    size_t start;
    size_t count;
};

/*
    The spsc ring data struct itself

    Accessible members:
        capacity -- the number of indexes managed by the ring
 */
struct ids_spsc_ring
{
    size_t capacity;

    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t write_cursor;
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t read_cursor;
};


/*
    Initialize an spsc ring.  The data struct will be empty upon initialization
 */
inline void
Ids_SpscRing_Init (size_t capacity, struct ids_spsc_ring* ring);

/*
    Reset an spsc ring to the initialized state
 */
inline void
Ids_SpscRing_Reset (struct ids_spsc_ring* ring);


/*
    Return the maximum number of indexes managed by the ring
 */
inline size_t
Ids_SpscRing_Capacity (struct ids_spsc_ring* ring);

/*
    Return the number of indexes available to the consumer
 */
inline size_t
Ids_SpscRing_Count (struct ids_spsc_ring* ring);

/*
    Return the number of indexes available to the producer
 */
inline size_t
Ids_SpscRing_Space (struct ids_spsc_ring* ring);

/*
    Return non-zero if the ring is empty
 */
inline int
Ids_SpscRing_Empty (struct ids_spsc_ring* ring);

/*
    Return non-zero if the ring is full
 */
inline int
Ids_SpscRing_Full (struct ids_spsc_ring* ring);


/*
    Reserve up to count contiguous indexes for the producer.  The returned count may be
    less than requested if the available producer range wraps or if the ring has less
    free space available.  If no space is available, the returned count is zero
 */
inline struct ids_spsc_ring_range
Ids_SpscRing_Reserve (size_t count, struct ids_spsc_ring* ring);

/*
    Commit previously reserved indexes to the consumer
 */
inline void
Ids_SpscRing_Commit (struct ids_spsc_ring_range* range, struct ids_spsc_ring* ring);


/*
    Return up to count contiguous indexes available to the consumer.  The returned count
    may be less than requested if the available consumer range wraps or if fewer indexes
    are available.  If the ring is empty, the returned count is zero
 */
inline struct ids_spsc_ring_range
Ids_SpscRing_Peek (size_t count, struct ids_spsc_ring* ring);

/*
    Release previously peeked indexes back to the producer
 */
inline void
Ids_SpscRing_Release (struct ids_spsc_ring_range* range, struct ids_spsc_ring* ring);


#include <ids/spsc_ring_inl.h>


#endif
