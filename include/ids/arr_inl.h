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


#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


inline void
Ids_Arr_Init (size_t elem_size, struct ids_arr* arr)
{
    arr->elem_size  = elem_size;
    arr->elem_count = 0;
    arr->capacity   = 0;
    arr->data       = NULL;
    arr->flags      = 0;
}

inline void
Ids_Arr_InitStatic (size_t elem_size,
                    size_t capacity,
                    void*  data,
                    struct ids_arr* arr)
{
    arr->elem_size  = elem_size;
    arr->elem_count = 0;
    arr->capacity   = capacity;
    arr->data       = data;
    arr->flags      = ids_arr_flag_static_data;
}

inline enum ids_err
Ids_Arr_Create (size_t elem_size, size_t capacity, struct ids_arr* arr)
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
    arr->flags     &= ~ids_arr_flag_static_data;

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
Ids_Arr_Count (struct ids_arr* arr)
{
    return arr->elem_count;
}

inline size_t
Ids_Arr_Capacity (struct ids_arr* arr)
{
    return arr->capacity;
}

inline void*
Ids_Arr_Data (struct ids_arr* arr)
{
    return arr->data;
}

inline enum ids_err
Ids_Arr_Reserve (size_t count, struct ids_arr* arr)
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

inline enum ids_err
Ids_Arr_Insert (void* data, size_t count, size_t elem_index, struct ids_arr* arr)
{
    size_t       new_count = elem_index + count;
    enum ids_err err       = Ids_Arr_Reserve(new_count, arr);
    if(err != ids_err_none)
        return err;

    size_t data_size     = count * arr->elem_size;
    size_t existing_size = elem_index * arr->elem_size;
    memcpy((unsigned char*)arr->data + existing_size, data, data_size);

    arr->elem_count = new_count;

    return ids_err_none;
}

inline enum ids_err
Ids_Arr_Add (void* data, size_t count, struct ids_arr* arr)
{
    return Ids_Arr_Insert(data, count, arr->elem_count, arr);
}

inline enum ids_err
Ids_Arr_Place (void* data, size_t count, struct ids_arr* arr)
{
    return Ids_Arr_Insert(data, count, 0, arr);
}
