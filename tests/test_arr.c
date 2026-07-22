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


#include <ids/arr.h>

#include <assert.h>
#include <stdlib.h>


static void
Test_Init (void)
{
    struct ids_arr arr;

    Ids_Arr_Init(sizeof(int), &arr);

    assert(Ids_Arr_Count(&arr) == 0);
    assert(Ids_Arr_Capacity(&arr) == 0);
    assert(Ids_Arr_Data(&arr) == NULL);
}

static void
Test_StaticStorage (void)
{
    int              data[2];
    int              values[3] = {1, 2, 3};
    struct ids_arr   arr;

    Ids_Arr_InitStatic(sizeof(int), 2, data, &arr);
    assert(Ids_Arr_Add(values, 2, &arr) == ids_err_none);
    assert(Ids_Arr_Data(&arr) == data);
    assert(Ids_Arr_Count(&arr) == 2);
    assert(data[0] == 1);
    assert(data[1] == 2);

    assert(Ids_Arr_Add(&values[2], 1, &arr) == ids_err_none);
    assert(Ids_Arr_Data(&arr) != data);
    assert(Ids_Arr_Count(&arr) == 3);
    assert(((int*)Ids_Arr_Data(&arr))[0] == 1);
    assert(((int*)Ids_Arr_Data(&arr))[1] == 2);
    assert(((int*)Ids_Arr_Data(&arr))[2] == 3);

    Ids_Arr_Destroy(&arr);
}

static void
Test_CreateReserve (void)
{
    int              values[4] = {1, 2, 3, 4};
    struct ids_arr   arr;
    void*            data;

    assert(Ids_Arr_Create(sizeof(int), 2, &arr) == ids_err_none);
    data = Ids_Arr_Data(&arr);
    assert(Ids_Arr_Capacity(&arr) == 2);

    assert(Ids_Arr_Reserve(4, &arr) == ids_err_none);
    assert(Ids_Arr_Capacity(&arr) >= 4);
    assert(Ids_Arr_Data(&arr) != NULL);
    assert(Ids_Arr_Data(&arr) != data || Ids_Arr_Capacity(&arr) > 2);

    assert(Ids_Arr_Add(values, 4, &arr) == ids_err_none);
    assert(Ids_Arr_Count(&arr) == 4);
    assert(((int*)Ids_Arr_Data(&arr))[3] == 4);

    Ids_Arr_Reset(&arr);
    assert(Ids_Arr_Count(&arr) == 0);
    assert(Ids_Arr_Capacity(&arr) >= 4);
    assert(Ids_Arr_Data(&arr) != NULL);

    Ids_Arr_Destroy(&arr);
}

static void
Test_ZeroCount (void)
{
    struct ids_arr arr;

    Ids_Arr_Init(sizeof(int), &arr);
    assert(Ids_Arr_Add(NULL, 0, &arr) == ids_err_none);
    assert(Ids_Arr_Count(&arr) == 0);
    assert(Ids_Arr_Data(&arr) == NULL);
}

int
main (void)
{
    Test_Init();
    Test_StaticStorage();
    Test_CreateReserve();
    Test_ZeroCount();

    return EXIT_SUCCESS;
}
