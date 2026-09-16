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

/*
    The mpsc stack data struct manages a lock-free multiple-producer-single-consumer stack
 */


#ifndef IDS_MPSC_STACK_H
#define IDS_MPSC_STACK_H


#include <stdatomic.h>
#include <stddef.h>


/*
    The node data to be placed in each mpsc stack element
 */
struct ids_mpsc_stack_node
{
    struct ids_mpsc_stack_node* next;
};

/*
    The mpsc stack data struct itself
 */
struct ids_mpsc_stack
{
    _Atomic(struct ids_mpsc_stack_node*) head;
};


/*
    Initialize an mpsc stack.  The data struct will be empty upon initialization
 */
inline void
Ids_MpscStack_Init (struct ids_mpsc_stack* stack);


/*
    Return non-zero if the mpsc stack is empty
 */
inline int
Ids_MpscStack_Empty (const struct ids_mpsc_stack* stack);


/*
    Push a node at the head of the mpsc stack
 */
inline void
Ids_MpscStack_Push (struct ids_mpsc_stack* stack, struct ids_mpsc_stack_node* node);

/*
    Pop the head node from the mpsc stack.  Return NULL if the mpsc stack is empty
 */
inline struct ids_mpsc_stack_node*
Ids_MpscStack_Pop (struct ids_mpsc_stack* stack);


#include <ids/mpsc_stack_inl.h>


#endif
