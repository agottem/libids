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


#ifndef IDS_ALG_H
#define IDS_ALG_H


#include <stddef.h>


/*
    Return less than zero if left sorts before right, zero if equivalent, or greater than
    zero if left sorts after right
 */
typedef int
ids_alg_cmp_t (const void* left, const void* right, void* user_data);


/*
    Insert elem into an ordered array of elem_count elements and return its index.  Storage
    must hold at least elem_count + 1 elements of elem_size bytes, and elem_size must be
    nonzero.  The caller updates the occupied count.  All insertions must use a consistent
    comparator ordering
 */
inline size_t
Ids_Alg_InsOrdered (void*          data,
                    size_t         elem_count,
                    size_t         elem_size,
                    const void*    elem,
                    ids_alg_cmp_t* cmp_func,
                    void*          user_data);


#include <ids/alg_inl.h>


#endif
