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
    The ring data struct manages single-threaded ring cursors
 */


#ifndef IDS_RING_H
#define IDS_RING_H


#include <stddef.h>


/*
    A contiguous reserved or peeked range

    Accessible members:
        start -- the first index
        count -- the number of indexes
 */
struct ids_ring_range
{
    size_t start;
    size_t count;
};

/*
    The ring data struct itself

    Accessible members:
        capacity -- the number of indexes managed by the ring
 */
struct ids_ring
{
    size_t capacity;

    size_t write_cursor;
    size_t read_cursor;
};


/*
    Initialize an empty ring.  Capacity must be nonzero and a power of two
 */
inline void
Ids_Ring_Init (struct ids_ring* ring, size_t capacity);

/*
    Reset the ring to the initialized state, retaining its capacity.  Outstanding
    ranges are invalidated
 */
inline void
Ids_Ring_Reset (struct ids_ring* ring);


/*
    Return the number of committed indexes
 */
inline size_t
Ids_Ring_Count (const struct ids_ring* ring);

/*
    Return the number of indexes available to reserve
 */
inline size_t
Ids_Ring_Space (const struct ids_ring* ring);

/*
    Return non-zero if the ring is empty
 */
inline int
Ids_Ring_Empty (const struct ids_ring* ring);

/*
    Return non-zero if the ring is full
 */
inline int
Ids_Ring_Full (const struct ids_ring* ring);


/*
    Return up to count contiguous writable indexes without advancing the ring.  The
    returned range may be shorter due to wrapping or limited space
 */
inline struct ids_ring_range
Ids_Ring_Reserve (const struct ids_ring* ring, size_t count);

/*
    Commit a previously reserved range
 */
inline void
Ids_Ring_Commit (struct ids_ring* ring, const struct ids_ring_range* range);


/*
    Return up to count contiguous committed indexes without advancing the ring.  The
    returned range may be shorter due to wrapping or limited availability
 */
inline struct ids_ring_range
Ids_Ring_Peek (const struct ids_ring* ring, size_t count);

/*
    Release a previously peeked range, making its indexes reusable
 */
inline void
Ids_Ring_Release (struct ids_ring* ring, const struct ids_ring_range* range);


#include <ids/ring_inl.h>


#endif
