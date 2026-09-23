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


#include <ids/arr.h>

#include <ids/err.h>


extern void
Ids_Arr_Init (struct ids_arr* arr, size_t elem_size);

extern void
Ids_Arr_InitStatic (struct ids_arr* arr,
                    size_t          elem_size,
                    size_t          capacity,
                    const void*     data);

extern enum ids_err
Ids_Arr_Create (struct ids_arr* arr, size_t elem_size, size_t capacity);

extern void
Ids_Arr_Destroy (struct ids_arr* arr);

extern void
Ids_Arr_Reset (struct ids_arr* arr);


extern size_t
Ids_Arr_Count (const struct ids_arr* arr);

extern size_t
Ids_Arr_Capacity (const struct ids_arr* arr);

extern void*
Ids_Arr_Data (const struct ids_arr* arr);

extern void*
Ids_Arr_At (const struct ids_arr* arr, size_t elem_index);


extern enum ids_err
Ids_Arr_Reserve (struct ids_arr* arr, size_t count);

extern void*
Ids_Arr_InsUninit (struct ids_arr* arr, size_t count, size_t elem_index);

extern enum ids_err
Ids_Arr_Ins (struct ids_arr* arr, const void* data, size_t count, size_t elem_index);

extern void*
Ids_Arr_AddUninit (struct ids_arr* arr, size_t count);

extern enum ids_err
Ids_Arr_Add (struct ids_arr* arr, const void* restrict data, size_t count);

extern enum ids_err
Ids_Arr_Place (struct ids_arr* arr, const void* restrict data, size_t count);
