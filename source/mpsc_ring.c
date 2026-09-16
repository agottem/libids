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


#include <ids/mpsc_ring.h>


extern void
Ids_MpscRing_Init (struct ids_mpsc_ring* ring, size_t capacity);

extern void
Ids_MpscRing_Reset (struct ids_mpsc_ring* ring);


extern size_t
Ids_MpscRing_Count (const struct ids_mpsc_ring* ring);

extern size_t
Ids_MpscRing_Space (const struct ids_mpsc_ring* ring);

extern int
Ids_MpscRing_Empty (const struct ids_mpsc_ring* ring);

extern int
Ids_MpscRing_Full (const struct ids_mpsc_ring* ring);


extern struct ids_mpsc_ring_range
Ids_MpscRing_Reserve (struct ids_mpsc_ring* ring, size_t count);

extern void
Ids_MpscRing_Commit (struct ids_mpsc_ring* ring, const struct ids_mpsc_ring_range* range);


extern struct ids_mpsc_ring_range
Ids_MpscRing_Peek (const struct ids_mpsc_ring* ring, size_t count);

extern void
Ids_MpscRing_Release (struct ids_mpsc_ring* ring, const struct ids_mpsc_ring_range* range);
