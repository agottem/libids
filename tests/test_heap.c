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


#include <ids/cont_of.h>
#include <ids/heap.h>

#include <stdint.h>
#include <stdlib.h>

#include "test_utils.h"


struct item
{
    int                  priority;
    int                  present;
    struct ids_heap_node node;
};

struct cmp_context
{
    int    direction;
    size_t calls;
};


static struct item*
NodeToItem (const struct ids_heap_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}

static int
ItemCmp (const struct ids_heap_node* left, const struct ids_heap_node* right, void* user_data)
{
    struct cmp_context* context   = user_data;
    int                 direction = 1;
    if(context != NULL)
    {
        context->calls++;
        direction = context->direction;
    }

    int a = NodeToItem(left)->priority;
    int b = NodeToItem(right)->priority;
    return ((a > b) - (a < b)) * direction;
}

/* Validate against caller-owned membership, independently of the embedded indexes. */
static void
ValidateHeap (const struct ids_heap* heap, struct item* items, size_t count, int direction)
{
    size_t expected = 0;
    for(size_t i = 0; i < count; i++)
    {
        size_t occurrences = 0;
        for(size_t j = 0; j < heap->node_count; j++)
            occurrences += heap->nodes[j] == &items[i].node ? 1u : 0u;
        CHECK(occurrences == (items[i].present ? 1u : 0u));
        expected += items[i].present ? 1u : 0u;
        if(items[i].present)
        {
            CHECK(Ids_Heap_Root(heap) != NULL);
            int root_priority = NodeToItem(Ids_Heap_Root(heap))->priority;
            CHECK(direction > 0 ? root_priority <= items[i].priority
                                : root_priority >= items[i].priority);
        }
    }

    CHECK(Ids_Heap_Count(heap) == expected);
    CHECK(expected <= Ids_Heap_Capacity(heap));
    CHECK(Ids_Heap_Empty(heap) == (expected == 0));
    CHECK(Ids_Heap_Full(heap) == (expected == heap->capacity));
    if(expected != 0)
        CHECK(Ids_Heap_Root(heap) != NULL);

    for(size_t i = 0; i < heap->node_count; i++)
    {
        CHECK(heap->nodes[i]->index == i);
        if(i > 0)
        {
            int parent = NodeToItem(heap->nodes[(i - 1) / 2])->priority;
            int child  = NodeToItem(heap->nodes[i])->priority;
            CHECK(direction > 0 ? parent <= child : parent >= child);
        }
    }
}

static void
Test_Lifecycle (void)
{
    struct ids_heap       heap;
    struct ids_heap_node* storage[2];
    struct item           items[2] = {{.priority = 3}, {.priority = 1}};

    Ids_Heap_Init(&heap, 0, NULL);
    ValidateHeap(&heap, items, 2, 1);
    CHECK(Ids_Heap_Full(&heap));

    Ids_Heap_Init(&heap, 2, storage);
    CHECK(heap.nodes == storage);
    for(size_t i = 0; i < 2; i++)
    {
        Ids_Heap_Add(&heap, &items[i].node, ItemCmp, NULL);
        items[i].present = 1;
    }
    ValidateHeap(&heap, items, 2, 1);

    Ids_Heap_Reset(&heap);
    CHECK(heap.nodes == storage);
    CHECK(heap.capacity == 2);
    for(size_t i = 0; i < 2; i++)
        items[i].present = 0;
    ValidateHeap(&heap, items, 2, 1);

    CHECK(Ids_Heap_Create(&heap, 2) == ids_err_none);
    for(size_t i = 0; i < 2; i++)
    {
        Ids_Heap_Add(&heap, &items[i].node, ItemCmp, NULL);
        items[i].present = 1;
        ValidateHeap(&heap, items, 2, 1);
    }
    while(!Ids_Heap_Empty(&heap))
    {
        NodeToItem(Ids_Heap_Pop(&heap, ItemCmp, NULL))->present = 0;
        ValidateHeap(&heap, items, 2, 1);
    }
    Ids_Heap_Destroy(&heap);
}

static void
Test_Orders (void)
{
    enum
    {
        item_count = 65
    };
    struct item     items[item_count];
    struct ids_heap heap;
    CHECK(Ids_Heap_Create(&heap, item_count) == ids_err_none);

    for(int direction = -1; direction <= 1; direction += 2)
    {
        struct cmp_context context = {.direction = direction};
        for(int order = 0; order < 4; order++)
        {
            for(size_t i = 0; i < item_count; i++)
            {
                items[i].present  = 0;
                items[i].priority = order == 0   ? (int)i
                                    : order == 1 ? -(int)i
                                    : order == 2 ? (int)(i % 5)
                                                 : 7;
            }
            for(size_t i = 0; i < item_count; i++)
            {
                Ids_Heap_Add(&heap, &items[i].node, ItemCmp, &context);
                items[i].present = 1;
                ValidateHeap(&heap, items, item_count, direction);
            }
            while(!Ids_Heap_Empty(&heap))
            {
                NodeToItem(Ids_Heap_Pop(&heap, ItemCmp, &context))->present = 0;
                ValidateHeap(&heap, items, item_count, direction);
            }
        }
    }
    Ids_Heap_Destroy(&heap);
}

static void
Test_DeleteUpdate (void)
{
    enum
    {
        item_count = 17
    };
    struct item           items[item_count] = {0};
    struct ids_heap_node* storage[item_count];
    struct ids_heap       heap;

    Ids_Heap_Init(&heap, item_count, storage);
    for(size_t i = 0; i < item_count; i++)
    {
        items[i].priority = (int)i;
        items[i].present  = 1;
        Ids_Heap_Add(&heap, &items[i].node, ItemCmp, NULL);
    }

    /* Change priorities at root, internal, and leaf positions in both directions. */
    size_t positions[] = {0, 3, 16};
    for(size_t i = 0; i < sizeof(positions) / sizeof(positions[0]); i++)
    {
        struct item* item = NodeToItem(heap.nodes[positions[i]]);
        item->priority    = -100;
        Ids_Heap_Update(&heap, &item->node, ItemCmp, NULL);
        CHECK(Ids_Heap_Root(&heap) == &item->node);
        ValidateHeap(&heap, items, item_count, 1);
        item->priority = 100;
        Ids_Heap_Update(&heap, &item->node, ItemCmp, NULL);
        ValidateHeap(&heap, items, item_count, 1);
        Ids_Heap_Update(&heap, &item->node, ItemCmp, NULL);
        ValidateHeap(&heap, items, item_count, 1);
    }

    size_t step = 0;
    while(!Ids_Heap_Empty(&heap))
    {
        size_t       index = step % 3 == 0   ? 0
                             : step % 3 == 1 ? heap.node_count / 2
                                             : heap.node_count - 1;
        struct item* item  = NodeToItem(heap.nodes[index]);
        Ids_Heap_Del(&heap, &item->node, ItemCmp, NULL);
        item->present = 0;
        ValidateHeap(&heap, items, item_count, 1);
        step++;
    }
}

static unsigned int
NextRandom (unsigned int* state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static void
Test_Stress (void)
{
    enum
    {
        item_count = 129
    };
    struct item     items[item_count] = {0};
    struct ids_heap heap;
    unsigned int    state = 0x7f4a7c15u;
    CHECK(Ids_Heap_Create(&heap, item_count) == ids_err_none);

    for(int direction = -1; direction <= 1; direction += 2)
    {
        struct cmp_context context = {.direction = direction};
        for(size_t step = 0; step < 5000; step++)
        {
            struct item* item      = &items[NextRandom(&state) % item_count];
            unsigned int operation = NextRandom(&state) >> 16;
            context.calls          = 0;
            if(!item->present)
            {
                item->priority = (int)(NextRandom(&state) % 101);
                Ids_Heap_Add(&heap, &item->node, ItemCmp, &context);
                item->present = 1;
            }
            else if(operation % 3 == 0)
            {
                Ids_Heap_Del(&heap, &item->node, ItemCmp, &context);
                item->present = 0;
            }
            else if(operation % 3 == 1)
                NodeToItem(Ids_Heap_Pop(&heap, ItemCmp, &context))->present = 0;
            else
            {
                item->priority = (int)(NextRandom(&state) % 101);
                Ids_Heap_Update(&heap, &item->node, ItemCmp, &context);
            }

            size_t levels = 0;
            for(size_t count = heap.node_count + 1; count > 0; count /= 2)
                levels++;
            CHECK(context.calls <= 2 * levels + 2);
            ValidateHeap(&heap, items, item_count, direction);
        }

        while(!Ids_Heap_Empty(&heap))
        {
            NodeToItem(Ids_Heap_Pop(&heap, ItemCmp, &context))->present = 0;
            ValidateHeap(&heap, items, item_count, direction);
        }
    }
    Ids_Heap_Destroy(&heap);
}

int
main (void)
{
    Test_Lifecycle();
    Test_Orders();
    Test_DeleteUpdate();
    Test_Stress();

    return EXIT_SUCCESS;
}
