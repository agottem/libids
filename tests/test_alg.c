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


#include <ids/alg.h>

#include <limits.h>
#include <stddef.h>

#include "test_utils.h"


struct item
{
    int    pri;
    size_t sequence;
};

struct cmp_context
{
    int    direction;
    size_t calls;
};


static int
ItemCmp (const void* left, const void* right, void* user_data)
{
    const struct item*  a       = left;
    const struct item*  b       = right;
    struct cmp_context* context = user_data;

    context->calls++;
    return ((a->pri > b->pri) - (a->pri < b->pri)) * context->direction;
}

static void
Test_Insertions (int direction)
{
    const int priorities[] = {5, INT_MIN, INT_MAX, 3, 5, 3, -1, 5};
    struct item storage[sizeof(priorities) / sizeof(priorities[0]) + 1];
    struct cmp_context context = {.direction = direction};
    size_t count = 0;

    storage[sizeof(priorities) / sizeof(priorities[0])] =
        (struct item){.pri = 123, .sequence = 456};

    for(size_t i = 0; i < sizeof(priorities) / sizeof(priorities[0]); i++)
    {
        struct item elem = {.pri = priorities[i], .sequence = i};
        size_t expected_index = 0;
        for(size_t j = 0; j < count; j++)
            if(direction > 0 ? storage[j].pri <= elem.pri : storage[j].pri >= elem.pri)
                expected_index++;

        context.calls = 0;
        size_t index = Ids_Alg_InsOrdered(storage, count, sizeof(storage[0]),
                                         &elem, ItemCmp, &context);
        CHECK(index == expected_index);
        CHECK(storage[index].pri == elem.pri);
        CHECK(storage[index].sequence == i);

        size_t max_calls = 0;
        for(size_t n = count; n != 0; n /= 2)
            max_calls++;
        CHECK(context.calls <= max_calls);
        count++;

        for(size_t j = 0; j < count; j++)
        {
            CHECK(storage[j].sequence <= i);
            CHECK(storage[j].pri == priorities[storage[j].sequence]);
            size_t occurrences = 0;
            for(size_t k = 0; k < count; k++)
                occurrences += storage[k].sequence == j ? 1u : 0u;
            CHECK(occurrences == 1);
            if(j == 0)
                continue;

            CHECK(direction > 0 ? storage[j - 1].pri <= storage[j].pri
                                : storage[j - 1].pri >= storage[j].pri);
            if(storage[j - 1].pri == storage[j].pri)
                CHECK(storage[j - 1].sequence < storage[j].sequence);
        }
    }

    CHECK(storage[count].pri == 123);
    CHECK(storage[count].sequence == 456);
}

static int
ByteCmp (const void* left, const void* right, void* user_data)
{
    const unsigned char* a = left;
    const unsigned char* b = right;

    return (*a > *b) - (*a < *b);
}

static void
Test_Bytes (void)
{
    unsigned char storage[256];
    for(size_t i = 0; i < sizeof(storage); i++)
    {
        unsigned char elem = (unsigned char)(255 - i);
        CHECK(Ids_Alg_InsOrdered(storage, i, sizeof(storage[0]),
                                 &elem, ByteCmp, NULL) == 0);
        for(size_t j = 0; j <= i; j++)
            CHECK(storage[j] == 255 - i + j);
    }
}

int
main (void)
{
    Test_Insertions(1);
    Test_Insertions(-1);
    Test_Bytes();

    return EXIT_SUCCESS;
}
