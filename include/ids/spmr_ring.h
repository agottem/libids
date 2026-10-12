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
    The spmr ring manages single-producer-multiple-reader ring cursors.  Every reader
    observes every committed index in producer order.  The caller owns the event storage
    and the fixed reader array, which must remain valid throughout use.  Each reader is
    exclusively operated by one thread.  The slowest reader limits producer space
 */


#ifndef IDS_SPMR_RING_H
#define IDS_SPMR_RING_H


#include <ids/err.h>
#include <ids/sys.h>

#include <stdalign.h>
#include <stdatomic.h>
#include <stddef.h>


/*
    A contiguous reserved or peeked range

    Accessible members:
        start -- the first index
        count -- the number of indexes
 */
struct ids_spmr_ring_range
{
    size_t start;
    size_t count;

    size_t cursor;
};

/*
    A reader cursor, initialized by the ring
 */
struct ids_spmr_ring_reader
{
    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t read_cursor;
};

/*
    The spmr ring itself

    Accessible members:
        capacity -- the number of indexes managed by the ring
        readers -- the fixed caller-owned reader array
        reader_count -- the number of readers
 */
struct ids_spmr_ring
{
    size_t                                     capacity;
    struct ids_spmr_ring_reader*               readers;
    size_t                                     reader_count;

    alignas(IDS_CACHE_LINE_SIZE) atomic_size_t write_cursor;
};


/*
    Initialize an empty ring and all readers.  Capacity and reader count must be nonzero.
    Capacity must be a power of two. This function requires exclusive access to the ring and readers
 */
inline void
Ids_SpmrRing_Init (struct ids_spmr_ring*        ring,
                   size_t                       capacity,
                   struct ids_spmr_ring_reader* readers,
                   size_t                       reader_count);

/*
    Reset the ring and every reader.  Requires exclusive access to the ring and readers
 */
inline void
Ids_SpmrRing_Reset (struct ids_spmr_ring* ring);


/*
    Return the number of committed indexes available to this reader.  Reader thread only
 */
inline size_t
Ids_SpmrRing_Count (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader);

/*
    Return a snapshot of producer space.  Scans all readers.  Producer thread only
 */
inline size_t
Ids_SpmrRing_Space (const struct ids_spmr_ring* ring);

/*
    Return non-zero if this reader has no committed indexes.  Reader thread only
 */
inline int
Ids_SpmrRing_Empty (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader);

/*
    Return non-zero if no producer space is available.  Producer thread only
 */
inline int
Ids_SpmrRing_Full (const struct ids_spmr_ring* ring);


/*
    Reserve up to count contiguous indexes.  Count must be nonzero.  The returned range
    may be shorter due to wrapping or limited space.  Returns a range with a zero count
    if no space is available.  Does not advance the cursor. Only the producer may reserve;
    reservations must not overlap in lifetime
 */
inline struct ids_spmr_ring_range
Ids_SpmrRing_Reserve (const struct ids_spmr_ring* ring, size_t count);

/*
    Publish a previously reserved range to every reader.  Producer thread only
 */
inline void
Ids_SpmrRing_Commit (struct ids_spmr_ring* ring, const struct ids_spmr_ring_range* range);


/*
    Return up to count contiguous committed indexes for this reader without advancing it.
    A zero count means no indexes are available.  Reader thread only.  Peeked ranges must
    not overlap in lifetime; event storage must not be modified by readers
 */
inline struct ids_spmr_ring_range
Ids_SpmrRing_Peek (const struct ids_spmr_ring*        ring,
                   const struct ids_spmr_ring_reader* reader,
                   size_t                             count);

/*
    Release a previously peeked range.  Reader thread only.  The reader must not access
    the range afterward.  Storage becomes reusable once every reader has released it
 */
inline void
Ids_SpmrRing_Release (struct ids_spmr_ring_reader* reader, const struct ids_spmr_ring_range* range);


#include <ids/spmr_ring_inl.h>


#endif
