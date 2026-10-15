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


#include <assert.h>
#include <stddef.h>
#include <string.h>


inline size_t
Ids_Alg_InsOrdered (void*          data,
                    size_t         elem_count,
                    size_t         elem_size,
                    const void*    elem,
                    ids_alg_cmp_t* cmp_func,
                    void*          user_data)
{
    assert(elem_size != 0 && "Element size must not be zero");

    unsigned char* bytes = data;
    size_t         first = 0;
    size_t         last  = elem_count;
    while(first < last)
    {
        size_t middle = first + (last - first) / 2;
        if((*cmp_func)(elem, bytes + middle * elem_size, user_data) < 0)
            last = middle;
        else
            first = middle + 1;
    }

    unsigned char* dest = bytes + first * elem_size;
    if(first != elem_count)
        memmove(dest + elem_size, dest, (elem_count - first) * elem_size);
    memcpy(dest, elem, elem_size);

    return first;
}
