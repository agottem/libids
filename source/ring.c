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


#include <ids/ring.h>

#include <stddef.h>


extern void
Ids_Ring_Init (struct ids_ring* ring, size_t capacity);

extern void
Ids_Ring_Reset (struct ids_ring* ring);


extern size_t
Ids_Ring_Count (const struct ids_ring* ring);

extern size_t
Ids_Ring_Space (const struct ids_ring* ring);

extern int
Ids_Ring_Empty (const struct ids_ring* ring);

extern int
Ids_Ring_Full (const struct ids_ring* ring);


extern struct ids_ring_range
Ids_Ring_Reserve (const struct ids_ring* ring, size_t count);

extern void
Ids_Ring_Commit (struct ids_ring* ring, const struct ids_ring_range* range);


extern struct ids_ring_range
Ids_Ring_Peek (const struct ids_ring* ring, size_t count);

extern void
Ids_Ring_Release (struct ids_ring* ring, const struct ids_ring_range* range);
