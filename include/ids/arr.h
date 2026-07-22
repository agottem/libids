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

/*
    The arr data struct manages a contiguous, dynamically growable array
 */


#ifndef IDS_ARR_H
#define IDS_ARR_H


#include <stddef.h>

#include <ids/err.h>


enum ids_arr_flags
{
    ids_arr_flag_static_data = 1 << 0,
};


/*
    The arr data struct itself

    Accessible members:
        elem_size -- the size of each array element in bytes
        elem_count -- the number of elements currently stored
        capacity -- the number of elements the current storage can hold
        data -- the array storage
 */
struct ids_arr
{
    size_t       elem_size;
    size_t       elem_count;
    size_t       capacity;
    void*        data;
    unsigned int flags;
};


/*
    Initialize an arr with no storage.  The data struct will be empty upon initialization
 */
inline void
Ids_Arr_Init (size_t elem_size, struct ids_arr* arr);

/*
    Initialize an arr with caller-provided storage.  The storage must remain valid for the
    lifetime of the arr.  The arr does not take ownership of the storage.
 */
inline void
Ids_Arr_InitStatic (size_t elem_size, size_t capacity, void* data, struct ids_arr* arr);

/*
    Allocate storage and initialize an arr.  The data struct will be empty upon initialization
 */
inline enum ids_err
Ids_Arr_Create (size_t elem_size, size_t capacity, struct ids_arr* arr);

/*
    Cleanup a previously created arr
 */
inline void
Ids_Arr_Destroy (struct ids_arr* arr);

/*
    Reset an arr to the initialized state while retaining its storage
 */
inline void
Ids_Arr_Reset (struct ids_arr* arr);


/*
    Return the number of elements currently stored in the arr
 */
inline size_t
Ids_Arr_Count (struct ids_arr* arr);

/*
    Return the number of elements the current storage can hold
 */
inline size_t
Ids_Arr_Capacity (struct ids_arr* arr);

/*
    Return the arr storage
 */
inline void*
Ids_Arr_Data (struct ids_arr* arr);


/*
    Ensure the arr can hold at least count elements
 */
inline enum ids_err
Ids_Arr_Reserve (size_t count, struct ids_arr* arr);

/*
    Insert data beginning at the specified element index
 */
inline enum ids_err
Ids_Arr_Insert (void* data, size_t count, size_t elem_index, struct ids_arr* arr);

/*
    Append count elements from data to the arr
 */
inline enum ids_err
Ids_Arr_Add (void* data, size_t count, struct ids_arr* arr);

/*
    Place count elements from data to the arr
 */
inline enum ids_err
Ids_Arr_Place (void* data, size_t count, struct ids_arr* arr);


#include <ids/arr_inl.h>


#endif
