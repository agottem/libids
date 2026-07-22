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


#ifndef IDS_CONT_OF_H
#define IDS_CONT_OF_H


#include <stddef.h>


/*
    Returns a pointer to the container of the specified type

    NOTE: The implementation is convoluted intentionally to suppress GCC's incorrect
          "no-strict-aliasing" warnings.
 */
#define IDS_CONT_OF(address, type, member)                            \
    ((type*)(1 ? ((char*)(address) - offsetof(type, member)) : NULL))


#endif
