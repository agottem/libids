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


#include <ids/alg.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


inline void
Ids_Arr_Init (struct ids_arr* arr, size_t elem_size)
{
    arr->elem_size  = elem_size;
    arr->elem_count = 0;
    arr->capacity   = 0;
    arr->data       = NULL;
    arr->flags      = ids_arr_flags_none;
}

inline void
Ids_Arr_InitStatic (struct ids_arr* arr,
                    size_t          elem_size,
                    size_t          capacity,
                    const void*     data)
{
    arr->elem_size  = elem_size;
    arr->elem_count = 0;
    arr->capacity   = capacity;
    arr->data       = (void*)data;
    arr->flags      = ids_arr_flag_static_data;
}

inline enum ids_err
Ids_Arr_Create (struct ids_arr* arr, size_t elem_size, size_t capacity)
{
    assert(elem_size != 0 && "Element size must not be zero");

    void* data = NULL;
    if(capacity > 0)
    {
        data = malloc(elem_size * capacity);
        if(data == NULL)
            return ids_err_mem;
    }

    arr->elem_size  = elem_size;
    arr->elem_count = 0;
    arr->capacity   = capacity;
    arr->data       = data;
    arr->flags      = ids_arr_flags_none;

    return ids_err_none;
}

inline void
Ids_Arr_Destroy (struct ids_arr* arr)
{
    if(!(arr->flags & ids_arr_flag_static_data))
        free(arr->data);
}

inline void
Ids_Arr_Reset (struct ids_arr* arr)
{
    arr->elem_count = 0;
}

inline size_t
Ids_Arr_Count (const struct ids_arr* arr)
{
    return arr->elem_count;
}

inline size_t
Ids_Arr_Capacity (const struct ids_arr* arr)
{
    return arr->capacity;
}

inline void*
Ids_Arr_Data (const struct ids_arr* arr)
{
    return arr->data;
}

inline void*
Ids_Arr_At (const struct ids_arr* arr, size_t elem_index)
{
    assert(elem_index < arr->elem_count && "Array element index must be in bounds");

    return (unsigned char*)arr->data + elem_index * arr->elem_size;
}

inline enum ids_err
Ids_Arr_Reserve (struct ids_arr* arr, size_t count)
{
    if(count <= arr->capacity)
        return ids_err_none;

    void*  data;
    size_t alloc_size = count * arr->elem_size;
    if(!(arr->flags & ids_arr_flag_static_data))
    {
        data = realloc(arr->data, alloc_size);
        if(data == NULL)
            return ids_err_mem;
    }
    else
    {
        data = malloc(alloc_size);
        if(data == NULL)
            return ids_err_mem;

        if(arr->elem_count != 0)
            memcpy(data, arr->data, arr->elem_count * arr->elem_size);

        arr->flags &= ~ids_arr_flag_static_data;
    }

    arr->data     = data;
    arr->capacity = count;

    return ids_err_none;
}

inline void*
Ids_Arr_InsUninit (struct ids_arr* arr, size_t count, size_t elem_index)
{
    assert(count > 0 && "Element count must be greater than zero");

    size_t       new_count = elem_index + count;
    enum ids_err err       = Ids_Arr_Reserve(arr, new_count);
    if(err != ids_err_none)
        return NULL;

    arr->elem_count = IDS_MAX(new_count, arr->elem_count);

    return Ids_Arr_At(arr, elem_index);
}

inline enum ids_err
Ids_Arr_Ins (struct ids_arr* arr, const void* data, size_t count, size_t elem_index)
{
    assert(count > 0 && "Element count must be greater than zero");

    void* insert_start = Ids_Arr_InsUninit(arr, count, elem_index);
    if(insert_start == NULL)
        return ids_err_mem;

    size_t insert_size = count * arr->elem_size;
    memcpy(insert_start, data, insert_size);

    return ids_err_none;
}

inline void*
Ids_Arr_AddUninit (struct ids_arr* arr, size_t count)
{
    assert(count > 0 && "Element count must be greater than zero");

    return Ids_Arr_InsUninit(arr, count, arr->elem_count);
}

inline enum ids_err
Ids_Arr_Add (struct ids_arr* arr, const void* restrict data, size_t count)
{
    assert(count > 0 && "Element count must be greater than zero");

    return Ids_Arr_Ins(arr, data, count, arr->elem_count);
}

inline enum ids_err
Ids_Arr_Place (struct ids_arr* arr, const void* restrict data, size_t count)
{
    assert(count > 0 && "Element count must be greater than zero");

    Ids_Arr_Reset(arr);
    return Ids_Arr_Ins(arr, data, count, 0);
}
