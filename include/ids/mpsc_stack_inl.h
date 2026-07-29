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


inline void
Ids_MpscStack_Init (struct ids_mpsc_stack* stack)
{
    atomic_init(&stack->head, NULL);
}

inline int
Ids_MpscStack_Empty (struct ids_mpsc_stack* stack)
{
    return atomic_load_explicit(&stack->head, memory_order_acquire) == NULL;
}

inline void
Ids_MpscStack_Push (struct ids_mpsc_stack_node* node, struct ids_mpsc_stack* stack)
{
    struct ids_mpsc_stack_node* head = atomic_load_explicit(&stack->head, memory_order_relaxed);
    do
    {
        node->next = head;
    } while(!atomic_compare_exchange_weak_explicit(&stack->head,
                                                   &head,
                                                   node,
                                                   memory_order_release,
                                                   memory_order_relaxed));
}

inline struct ids_mpsc_stack_node*
Ids_MpscStack_Pop (struct ids_mpsc_stack* stack)
{
    struct ids_mpsc_stack_node* head = atomic_load_explicit(&stack->head, memory_order_acquire);
    while (head != NULL)
    {
        struct ids_mpsc_stack_node* next = head->next;
        if(atomic_compare_exchange_weak_explicit(&stack->head,
                                                 &head,
                                                 next,
                                                 memory_order_acquire,
                                                 memory_order_acquire))
        {
            return head;
        }
    }

    return NULL;
}
