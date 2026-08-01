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
Ids_MpscRing_Init (size_t capacity, struct ids_mpsc_ring* ring);

extern void
Ids_MpscRing_Reset (struct ids_mpsc_ring* ring);


extern size_t
Ids_MpscRing_Count (struct ids_mpsc_ring* ring);

extern size_t
Ids_MpscRing_Space (struct ids_mpsc_ring* ring);

extern int
Ids_MpscRing_Empty (struct ids_mpsc_ring* ring);

extern int
Ids_MpscRing_Full (struct ids_mpsc_ring* ring);


extern struct ids_mpsc_ring_range
Ids_MpscRing_Reserve (size_t count, struct ids_mpsc_ring* ring);

extern void
Ids_MpscRing_Commit (struct ids_mpsc_ring_range* range, struct ids_mpsc_ring* ring);


extern struct ids_mpsc_ring_range
Ids_MpscRing_Peek (size_t count, struct ids_mpsc_ring* ring);

extern void
Ids_MpscRing_Release (struct ids_mpsc_ring_range* range, struct ids_mpsc_ring* ring);
