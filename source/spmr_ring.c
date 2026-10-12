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


#include <ids/spmr_ring.h>

#include <stddef.h>


extern void
Ids_SpmrRing_Init (struct ids_spmr_ring*        ring,
                   size_t                       capacity,
                   struct ids_spmr_ring_reader* readers,
                   size_t                       reader_count);

extern void
Ids_SpmrRing_Reset (struct ids_spmr_ring* ring);


extern size_t
Ids_SpmrRing_Count (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader);

extern size_t
Ids_SpmrRing_Space (const struct ids_spmr_ring* ring);

extern int
Ids_SpmrRing_Empty (const struct ids_spmr_ring* ring, const struct ids_spmr_ring_reader* reader);

extern int
Ids_SpmrRing_Full (const struct ids_spmr_ring* ring);


extern struct ids_spmr_ring_range
Ids_SpmrRing_Reserve (const struct ids_spmr_ring* ring, size_t count);

extern void
Ids_SpmrRing_Commit (struct ids_spmr_ring* ring, const struct ids_spmr_ring_range* range);


extern struct ids_spmr_ring_range
Ids_SpmrRing_Peek (const struct ids_spmr_ring*        ring,
                   const struct ids_spmr_ring_reader* reader,
                   size_t                             count);

extern void
Ids_SpmrRing_Release (struct ids_spmr_ring_reader* reader, const struct ids_spmr_ring_range* range);
