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


#include <ids/rbtree.h>
#include <ids/cont_of.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>


struct item
{
    int                    id;
    struct ids_rbtree_node node;
};


static inline struct item*
NodeToItem (struct ids_rbtree_node* node)
{
    return IDS_CONT_OF(node, struct item, node);
}

static int
ItemCmp (void* id, struct ids_rbtree_node* node, void* user_data)
{
    int direction = user_data == NULL ? 1 : *(int*)user_data;
    int node_id   = NodeToItem(node)->id;

    if(*(int*)id < node_id)
        return -direction;
    if(*(int*)id > node_id)
        return direction;

    return 0;
}

static void
AddItem (struct item* item, struct ids_rbtree* rbtree)
{
    assert(Ids_RbTree_Add(&item->id, &item->node, rbtree, ItemCmp, NULL) == NULL);
}

static int
ValidateNode (struct ids_rbtree_node* node,
              struct ids_rbtree_node* parent,
              int*                    previous,
              int*                    count)
{
    if(node == NULL)
        return 1;

    assert(node->parent == parent);
    if(node->color == ids_rbtree_color_red)
    {
        assert(node->left == NULL || node->left->color == ids_rbtree_color_black);
        assert(node->right == NULL || node->right->color == ids_rbtree_color_black);
    }

    int left_height = ValidateNode(node->left, node, previous, count);
    int id          = NodeToItem(node)->id;
    if(*count > 0)
        assert(*previous < id);
    *previous = id;
    (*count)++;

    int right_height = ValidateNode(node->right, node, previous, count);
    assert(left_height == right_height);

    return left_height + (node->color == ids_rbtree_color_black);
}

static void
ValidateTree (int expected_count, struct ids_rbtree* rbtree)
{
    int count    = 0;
    int previous = 0;

    if(rbtree->root != NULL)
    {
        assert(rbtree->root->parent == NULL);
        assert(rbtree->root->color == ids_rbtree_color_black);
    }

    ValidateNode(rbtree->root, NULL, &previous, &count);
    assert(count == expected_count);
}

static void
Test_InitReset (void)
{
    struct ids_rbtree rbtree;
    struct item       item = {.id = 1};

    Ids_RbTree_Init(&rbtree);
    assert(Ids_RbTree_Empty(&rbtree));
    assert(Ids_RbTree_Root(&rbtree) == NULL);
    assert(Ids_RbTree_Min(&rbtree) == NULL);
    assert(Ids_RbTree_Max(&rbtree) == NULL);

    AddItem(&item, &rbtree);
    assert(!Ids_RbTree_Empty(&rbtree));
    assert(Ids_RbTree_Root(&rbtree) == &item.node);

    Ids_RbTree_Reset(&rbtree);
    assert(Ids_RbTree_Empty(&rbtree));
}

static void
Test_Rotations (void)
{
    int orders[][3] = {
        {3, 2, 1},
        {1, 2, 3},
        {3, 1, 2},
        {1, 3, 2},
    };

    for(size_t order = 0; order < sizeof(orders) / sizeof(orders[0]); order++)
    {
        struct ids_rbtree rbtree;
        struct item       items[3];

        Ids_RbTree_Init(&rbtree);
        for(int i = 0; i < 3; i++)
        {
            items[i].id = orders[order][i];
            AddItem(&items[i], &rbtree);
            ValidateTree(i + 1, &rbtree);
        }

        assert(NodeToItem(rbtree.root)->id == 2);
    }
}

static void
Test_FindBoundsDuplicate (void)
{
    int               ids[] = {10, 20, 30, 40};
    struct item       items[4];
    struct item       duplicate = {.id = 20};
    struct ids_rbtree rbtree;

    Ids_RbTree_Init(&rbtree);
    for(int i = 0; i < 4; i++)
    {
        items[i].id = ids[i];
        AddItem(&items[i], &rbtree);
    }

    int id = 20;
    assert(Ids_RbTree_Find(&id, &rbtree, ItemCmp, NULL) == &items[1].node);
    assert(Ids_RbTree_Add(&id, &duplicate.node, &rbtree, ItemCmp, NULL) == &items[1].node);
    ValidateTree(4, &rbtree);

    id = 25;
    assert(Ids_RbTree_Find(&id, &rbtree, ItemCmp, NULL) == NULL);
    assert(NodeToItem(Ids_RbTree_LowerBound(&id, &rbtree, ItemCmp, NULL))->id == 30);
    assert(NodeToItem(Ids_RbTree_UpperBound(&id, &rbtree, ItemCmp, NULL))->id == 30);

    id = 20;
    assert(NodeToItem(Ids_RbTree_LowerBound(&id, &rbtree, ItemCmp, NULL))->id == 20);
    assert(NodeToItem(Ids_RbTree_UpperBound(&id, &rbtree, ItemCmp, NULL))->id == 30);

    id = 5;
    assert(NodeToItem(Ids_RbTree_LowerBound(&id, &rbtree, ItemCmp, NULL))->id == 10);
    assert(NodeToItem(Ids_RbTree_UpperBound(&id, &rbtree, ItemCmp, NULL))->id == 10);

    id = 40;
    assert(NodeToItem(Ids_RbTree_LowerBound(&id, &rbtree, ItemCmp, NULL))->id == 40);
    assert(Ids_RbTree_UpperBound(&id, &rbtree, ItemCmp, NULL) == NULL);

    id = 50;
    assert(Ids_RbTree_LowerBound(&id, &rbtree, ItemCmp, NULL) == NULL);
    assert(Ids_RbTree_UpperBound(&id, &rbtree, ItemCmp, NULL) == NULL);
}

static void
Test_UserData (void)
{
    int               direction = -1;
    struct item       a         = {.id = 1};
    struct item       b         = {.id = 2};
    struct item       c         = {.id = 3};
    struct ids_rbtree rbtree;

    Ids_RbTree_Init(&rbtree);
    assert(Ids_RbTree_Add(&a.id, &a.node, &rbtree, ItemCmp, &direction) == NULL);
    assert(Ids_RbTree_Add(&b.id, &b.node, &rbtree, ItemCmp, &direction) == NULL);
    assert(Ids_RbTree_Add(&c.id, &c.node, &rbtree, ItemCmp, &direction) == NULL);
    assert(Ids_RbTree_Find(&b.id, &rbtree, ItemCmp, &direction) == &b.node);
    assert(NodeToItem(Ids_RbTree_Min(&rbtree))->id == 3);
    assert(NodeToItem(Ids_RbTree_Max(&rbtree))->id == 1);
}

static void
Test_It (void)
{
    int                  ids[] = {4, 2, 6, 1, 3, 5, 7};
    struct item          items[7];
    struct ids_rbtree    rbtree;
    struct ids_rbtree_it it;

    Ids_RbTree_Init(&rbtree);
    Ids_RbTree_BeginIt(&rbtree, &it);
    assert(Ids_RbTree_ItDone(&it));

    for(int i = 0; i < 7; i++)
    {
        items[i].id = ids[i];
        AddItem(&items[i], &rbtree);
    }

    int expected = 1;
    for(Ids_RbTree_BeginIt(&rbtree, &it); !Ids_RbTree_ItDone(&it); Ids_RbTree_ItFwd(&it))
    {
        assert(NodeToItem(it.node)->id == expected);
        expected++;
    }
    assert(expected == 8);

    expected = 7;
    for(Ids_RbTree_RBeginIt(&rbtree, &it); !Ids_RbTree_ItDone(&it); Ids_RbTree_ItBack(&it))
    {
        assert(NodeToItem(it.node)->id == expected);
        expected--;
    }
    assert(expected == 0);

    Ids_RbTree_NBeginIt(&items[4].node, &it);
    assert(NodeToItem(it.node)->id == 3);
    Ids_RbTree_ItFwd(&it);
    assert(NodeToItem(it.node)->id == 4);
    Ids_RbTree_ItBack(&it);
    assert(NodeToItem(it.node)->id == 3);
}

static void
Test_DeleteCases (void)
{
    int               ids[]     = {20, 10, 30, 5, 15, 25, 40, 22, 27, 35, 50};
    int               deletes[] = {5, 25, 30, 20, 50, 10, 22, 15, 35, 27, 40};
    struct item       items[11];
    struct ids_rbtree rbtree;

    Ids_RbTree_Init(&rbtree);
    for(int i = 0; i < 11; i++)
    {
        items[i].id = ids[i];
        AddItem(&items[i], &rbtree);
        ValidateTree(i + 1, &rbtree);
    }

    for(int i = 0; i < 11; i++)
    {
        struct ids_rbtree_node* node = Ids_RbTree_Find(&deletes[i], &rbtree, ItemCmp, NULL);
        assert(node != NULL);
        Ids_RbTree_Del(node, &rbtree);
        assert(Ids_RbTree_Find(&deletes[i], &rbtree, ItemCmp, NULL) == NULL);
        ValidateTree(10 - i, &rbtree);
    }

    assert(Ids_RbTree_Empty(&rbtree));

    items[0].id = 100;
    AddItem(&items[0], &rbtree);
    assert(Ids_RbTree_Root(&rbtree) == &items[0].node);
    Ids_RbTree_Del(&items[0].node, &rbtree);
    assert(Ids_RbTree_Empty(&rbtree));
}

static void
Test_Stress (void)
{
    enum
    {
        item_count = 1000
    };
    struct item*      items = malloc(sizeof(*items) * item_count);
    int*              order = malloc(sizeof(*order) * item_count);
    struct ids_rbtree rbtree;

    assert(items != NULL);
    assert(order != NULL);
    Ids_RbTree_Init(&rbtree);

    for(int i = 0; i < item_count; i++)
    {
        order[i]    = i;
        items[i].id = i;
        AddItem(&items[i], &rbtree);
        ValidateTree(i + 1, &rbtree);
    }

    unsigned int state = 0x7f4a7c15u;
    for(int i = item_count - 1; i > 0; i--)
    {
        state             = state * 1664525u + 1013904223u;
        int swap_index    = (int)(state % (unsigned int)(i + 1));
        int value         = order[i];
        order[i]          = order[swap_index];
        order[swap_index] = value;
    }

    for(int i = 0; i < item_count; i++)
    {
        Ids_RbTree_Del(&items[order[i]].node, &rbtree);
        ValidateTree(item_count - i - 1, &rbtree);
    }

    free(order);
    free(items);
}

static void
Test_PostIt (void)
{
    enum
    {
        item_count = 127
    };
    struct ids_rbtree         rbtree;
    struct ids_rbtree_post_it it;

    Ids_RbTree_Init(&rbtree);
    Ids_RbTree_BeginPostIt(&rbtree, &it);
    assert(Ids_RbTree_PostItDone(&it));

    for(int i = 0; i < item_count; i++)
    {
        struct item* item = malloc(sizeof(*item));
        assert(item != NULL);
        item->id = (i * 53) % item_count;
        AddItem(item, &rbtree);
    }

    int visited[item_count] = {0};
    int count               = 0;
    Ids_RbTree_BeginPostIt(&rbtree, &it);
    while(!Ids_RbTree_PostItDone(&it))
    {
        struct ids_rbtree_node* node = it.node;
        struct item*            item = NodeToItem(node);

        assert(!visited[item->id]);
        visited[item->id] = 1;
        count++;

        Ids_RbTree_PostItFwd(&it);
        free(item);
    }

    assert(count == item_count);
    Ids_RbTree_Reset(&rbtree);
    assert(Ids_RbTree_Empty(&rbtree));
}

int
main (void)
{
    Test_InitReset();
    Test_Rotations();
    Test_FindBoundsDuplicate();
    Test_UserData();
    Test_It();
    Test_DeleteCases();
    Test_Stress();
    Test_PostIt();

    return EXIT_SUCCESS;
}
