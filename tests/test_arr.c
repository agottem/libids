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
    assert(Ids_Arr_Count(&arr) == 0);
    assert(Ids_Arr_Data(&arr) == NULL);
}

static void
Test_At (void)
{
    int            values[3] = {1, 2, 3};
    struct ids_arr arr;

    Ids_Arr_InitStatic(sizeof(int), 3, values, &arr);
    assert(Ids_Arr_Add(values, 3, &arr) == ids_err_none);

    assert(Ids_Arr_At(0, &arr) == &values[0]);
    assert(Ids_Arr_At(2, &arr) == &values[2]);
    assert(*(int*)Ids_Arr_At(1, &arr) == 2);

    *(int*)Ids_Arr_At(1, &arr) = 4;
    assert(values[1] == 4);
}

static void
Test_AddUninit (void)
{
    int            storage[3];
    struct ids_arr arr;
    int*           values;

    Ids_Arr_InitStatic(sizeof(int), 3, storage, &arr);

    values = Ids_Arr_AddUninit(2, &arr);
    assert(values == storage);
    assert(Ids_Arr_Count(&arr) == 2);
    values[0] = 1;
    values[1] = 2;

    values = Ids_Arr_AddUninit(1, &arr);
    assert(values == &storage[2]);
    assert(Ids_Arr_Count(&arr) == 3);
    values[0] = 3;

    values = Ids_Arr_AddUninit(2, &arr);
    assert(values != NULL);
    assert(values == Ids_Arr_At(3, &arr));
    assert(Ids_Arr_Data(&arr) != storage);
    assert(Ids_Arr_Count(&arr) == 5);
    values[0] = 4;
    values[1] = 5;

    assert(*(int*)Ids_Arr_At(0, &arr) == 1);
    assert(*(int*)Ids_Arr_At(4, &arr) == 5);

    Ids_Arr_Destroy(&arr);
}

int
main (void)
{
    Test_Init();
    Test_StaticStorage();
    Test_CreateReserve();
    Test_ZeroCount();
    Test_At();
    Test_AddUninit();

    return EXIT_SUCCESS;
}
