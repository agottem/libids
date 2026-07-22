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


extern void
Ids_Arr_Init (size_t elem_size, struct ids_arr* arr);

extern void
Ids_Arr_InitStatic (size_t elem_size,
                    size_t capacity,
                    void*  data,
                    struct ids_arr* arr);

extern enum ids_err
Ids_Arr_Create (size_t elem_size, size_t capacity, struct ids_arr* arr);

extern void
Ids_Arr_Destroy (struct ids_arr* arr);

extern void
Ids_Arr_Reset (struct ids_arr* arr);


extern size_t
Ids_Arr_Count (struct ids_arr* arr);

extern size_t
Ids_Arr_Capacity (struct ids_arr* arr);

extern void*
Ids_Arr_Data (struct ids_arr* arr);


extern enum ids_err
Ids_Arr_Reserve (size_t count, struct ids_arr* arr);

extern enum ids_err
Ids_Arr_Insert (void* data, size_t count, size_t elem_index, struct ids_arr* arr);

extern enum ids_err
Ids_Arr_Add (void* data, size_t count, struct ids_arr* arr);

extern enum ids_err
Ids_Arr_Place (void* data, size_t count, struct ids_arr* arr);
