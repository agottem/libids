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


#include <stddef.h>


inline enum ids_rbtree_color
Ids_RbTree_Color (const struct ids_rbtree_node* node)
{
    return node == NULL ? ids_rbtree_color_black : node->color;
}

inline struct ids_rbtree_node*
Ids_RbTree_NodeMin (const struct ids_rbtree_node* node)
{
    if(node == NULL)
        return NULL;

    const struct ids_rbtree_node* scan = node;
    while(scan->left != NULL)
        scan = scan->left;

    return (struct ids_rbtree_node*)scan;
}

inline struct ids_rbtree_node*
Ids_RbTree_NodeMax (const struct ids_rbtree_node* node)
{
    if(node == NULL)
        return NULL;

    const struct ids_rbtree_node* scan = node;
    while(scan->right != NULL)
        scan = scan->right;

    return (struct ids_rbtree_node*)scan;
}

inline struct ids_rbtree_node*
Ids_RbTree_NodeNext (const struct ids_rbtree_node* node)
{
    if(node->right != NULL)
        return Ids_RbTree_NodeMin(node->right);

    const struct ids_rbtree_node* child  = node;
    const struct ids_rbtree_node* parent = child->parent;
    while(parent != NULL && child == parent->right)
    {
        child  = parent;
        parent = parent->parent;
    }

    return (struct ids_rbtree_node*)parent;
}

inline struct ids_rbtree_node*
Ids_RbTree_NodePrev (const struct ids_rbtree_node* node)
{
    if(node->left != NULL)
        return Ids_RbTree_NodeMax(node->left);

    const struct ids_rbtree_node* child  = node;
    const struct ids_rbtree_node* parent = node->parent;
    while(parent != NULL && child == parent->left)
    {
        child  = parent;
        parent = parent->parent;
    }

    return (struct ids_rbtree_node*)parent;
}

inline struct ids_rbtree_node*
Ids_RbTree_PostFirst (const struct ids_rbtree_node* node)
{
    if(node == NULL)
        return NULL;

    const struct ids_rbtree_node* scan = node;
    while(scan->left != NULL || scan->right != NULL)
    {
        if(scan->left != NULL)
            scan = scan->left;
        else
            scan = scan->right;
    }

    return (struct ids_rbtree_node*)scan;
}

inline struct ids_rbtree_node*
Ids_RbTree_PostNext (const struct ids_rbtree_node* node)
{
    const struct ids_rbtree_node* parent = node->parent;
    if(parent == NULL)
        return NULL;

    if(node == parent->left && parent->right != NULL)
        return Ids_RbTree_PostFirst(parent->right);

    return (struct ids_rbtree_node*)parent;
}

inline void
Ids_RbTree_RotateLeft (struct ids_rbtree* rbtree, struct ids_rbtree_node* node)
{
    struct ids_rbtree_node* pivot = node->right;

    node->right = pivot->left;
    if(pivot->left != NULL)
        pivot->left->parent = node;

    pivot->parent = node->parent;
    if(node->parent == NULL)
        rbtree->root = pivot;
    else if(node == node->parent->left)
        node->parent->left = pivot;
    else
        node->parent->right = pivot;

    pivot->left  = node;
    node->parent = pivot;
}

inline void
Ids_RbTree_RotateRight (struct ids_rbtree* rbtree, struct ids_rbtree_node* node)
{
    struct ids_rbtree_node* pivot = node->left;

    node->left = pivot->right;
    if(pivot->right != NULL)
        pivot->right->parent = node;

    pivot->parent = node->parent;
    if(node->parent == NULL)
        rbtree->root = pivot;
    else if(node == node->parent->right)
        node->parent->right = pivot;
    else
        node->parent->left = pivot;

    pivot->right = node;
    node->parent = pivot;
}

inline void
Ids_RbTree_FixAdd (struct ids_rbtree* rbtree, struct ids_rbtree_node* node)
{
    while(node->parent != NULL && node->parent->color == ids_rbtree_color_red)
    {
        struct ids_rbtree_node* parent      = node->parent;
        struct ids_rbtree_node* grandparent = parent->parent;

        if(parent == grandparent->left)
        {
            struct ids_rbtree_node* uncle = grandparent->right;
            if(Ids_RbTree_Color(uncle) == ids_rbtree_color_red)
            {
                parent->color      = ids_rbtree_color_black;
                uncle->color       = ids_rbtree_color_black;
                grandparent->color = ids_rbtree_color_red;
                node               = grandparent;
                continue;
            }

            if(node == parent->right)
            {
                node = parent;
                Ids_RbTree_RotateLeft(rbtree, node);
                parent = node->parent;
            }

            parent->color         = ids_rbtree_color_black;
            parent->parent->color = ids_rbtree_color_red;
            Ids_RbTree_RotateRight(rbtree, parent->parent);
        }
        else
        {
            struct ids_rbtree_node* uncle = grandparent->left;
            if(Ids_RbTree_Color(uncle) == ids_rbtree_color_red)
            {
                parent->color      = ids_rbtree_color_black;
                uncle->color       = ids_rbtree_color_black;
                grandparent->color = ids_rbtree_color_red;
                node               = grandparent;
                continue;
            }

            if(node == parent->left)
            {
                node = parent;
                Ids_RbTree_RotateRight(rbtree, node);
                parent = node->parent;
            }

            parent->color         = ids_rbtree_color_black;
            parent->parent->color = ids_rbtree_color_red;
            Ids_RbTree_RotateLeft(rbtree, parent->parent);
        }
    }

    rbtree->root->color = ids_rbtree_color_black;
}

inline void
Ids_RbTree_Replace (struct ids_rbtree*      rbtree,
                    struct ids_rbtree_node* node,
                    struct ids_rbtree_node* replacement)
{
    if(node->parent == NULL)
        rbtree->root = replacement;
    else if(node == node->parent->left)
        node->parent->left = replacement;
    else
        node->parent->right = replacement;

    if(replacement != NULL)
        replacement->parent = node->parent;
}

inline void
Ids_RbTree_FixDel (struct ids_rbtree*      rbtree,
                   struct ids_rbtree_node* node,
                   struct ids_rbtree_node* parent)
{
    while(node != rbtree->root && Ids_RbTree_Color(node) == ids_rbtree_color_black)
    {
        if(node == parent->left)
        {
            struct ids_rbtree_node* sibling = parent->right;
            if(Ids_RbTree_Color(sibling) == ids_rbtree_color_red)
            {
                sibling->color = ids_rbtree_color_black;
                parent->color  = ids_rbtree_color_red;
                Ids_RbTree_RotateLeft(rbtree, parent);
                sibling = parent->right;
            }

            if(Ids_RbTree_Color(sibling->left) == ids_rbtree_color_black &&
               Ids_RbTree_Color(sibling->right) == ids_rbtree_color_black)
            {
                sibling->color = ids_rbtree_color_red;
                node           = parent;
                parent         = node->parent;
            }
            else
            {
                if(Ids_RbTree_Color(sibling->right) == ids_rbtree_color_black)
                {
                    if(sibling->left != NULL)
                        sibling->left->color = ids_rbtree_color_black;
                    sibling->color = ids_rbtree_color_red;
                    Ids_RbTree_RotateRight(rbtree, sibling);
                    sibling = parent->right;
                }

                sibling->color = parent->color;
                parent->color  = ids_rbtree_color_black;
                if(sibling->right != NULL)
                    sibling->right->color = ids_rbtree_color_black;
                Ids_RbTree_RotateLeft(rbtree, parent);
                node = rbtree->root;
                break;
            }
        }
        else
        {
            struct ids_rbtree_node* sibling = parent->left;
            if(Ids_RbTree_Color(sibling) == ids_rbtree_color_red)
            {
                sibling->color = ids_rbtree_color_black;
                parent->color  = ids_rbtree_color_red;
                Ids_RbTree_RotateRight(rbtree, parent);
                sibling = parent->left;
            }

            if(Ids_RbTree_Color(sibling->right) == ids_rbtree_color_black &&
               Ids_RbTree_Color(sibling->left) == ids_rbtree_color_black)
            {
                sibling->color = ids_rbtree_color_red;
                node           = parent;
                parent         = node->parent;
            }
            else
            {
                if(Ids_RbTree_Color(sibling->left) == ids_rbtree_color_black)
                {
                    if(sibling->right != NULL)
                        sibling->right->color = ids_rbtree_color_black;
                    sibling->color = ids_rbtree_color_red;
                    Ids_RbTree_RotateLeft(rbtree, sibling);
                    sibling = parent->left;
                }

                sibling->color = parent->color;
                parent->color  = ids_rbtree_color_black;
                if(sibling->left != NULL)
                    sibling->left->color = ids_rbtree_color_black;
                Ids_RbTree_RotateRight(rbtree, parent);
                node = rbtree->root;
                break;
            }
        }
    }

    if(node != NULL)
        node->color = ids_rbtree_color_black;
}


inline void
Ids_RbTree_Init (struct ids_rbtree* rbtree)
{
    rbtree->root = NULL;
}

inline void
Ids_RbTree_Reset (struct ids_rbtree* rbtree)
{
    Ids_RbTree_Init(rbtree);
}

inline int
Ids_RbTree_Empty (const struct ids_rbtree* rbtree)
{
    return rbtree->root == NULL;
}

inline struct ids_rbtree_node*
Ids_RbTree_Root (const struct ids_rbtree* rbtree)
{
    return rbtree->root;
}

inline struct ids_rbtree_node*
Ids_RbTree_Min (const struct ids_rbtree* rbtree)
{
    return Ids_RbTree_NodeMin(rbtree->root);
}

inline struct ids_rbtree_node*
Ids_RbTree_Max (const struct ids_rbtree* rbtree)
{
    return Ids_RbTree_NodeMax(rbtree->root);
}

inline struct ids_rbtree_node*
Ids_RbTree_Find (const struct ids_rbtree* rbtree,
                 const void*              id,
                 ids_rbtree_cmp_t*        cmp_func,
                 void*                    user_data)
{
    const struct ids_rbtree_node* node = rbtree->root;
    while(node != NULL)
    {
        int delta = (*cmp_func)(id, node, user_data);
        if(delta == 0)
            return (struct ids_rbtree_node*)node;

        node = delta < 0 ? node->left : node->right;
    }

    return NULL;
}

inline struct ids_rbtree_node*
Ids_RbTree_LowerBound (const struct ids_rbtree* rbtree,
                       const void*              id,
                       ids_rbtree_cmp_t*        cmp_func,
                       void*                    user_data)
{
    const struct ids_rbtree_node* bound = NULL;
    const struct ids_rbtree_node* node  = rbtree->root;
    while(node != NULL)
    {
        int delta = (*cmp_func)(id, node, user_data);
        if(delta <= 0)
        {
            bound = node;
            node  = node->left;
        }
        else
            node = node->right;
    }

    return (struct ids_rbtree_node*)bound;
}

inline struct ids_rbtree_node*
Ids_RbTree_UpperBound (const struct ids_rbtree* rbtree,
                       const void*              id,
                       ids_rbtree_cmp_t*        cmp_func,
                       void*                    user_data)
{
    const struct ids_rbtree_node* bound = NULL;
    const struct ids_rbtree_node* node  = rbtree->root;
    while(node != NULL)
    {
        int delta = (*cmp_func)(id, node, user_data);
        if(delta < 0)
        {
            bound = node;
            node  = node->left;
        }
        else
            node = node->right;
    }

    return (struct ids_rbtree_node*)bound;
}

inline struct ids_rbtree_node*
Ids_RbTree_Add (struct ids_rbtree*      rbtree,
                const void*             id,
                struct ids_rbtree_node* node,
                ids_rbtree_cmp_t*       cmp_func,
                void*                   user_data)
{
    struct ids_rbtree_node*  parent = NULL;
    struct ids_rbtree_node** link   = &rbtree->root;

    while(*link != NULL)
    {
        int delta;

        parent = *link;
        delta  = (*cmp_func)(id, parent, user_data);
        if(delta == 0)
            return parent;

        link = delta < 0 ? &parent->left : &parent->right;
    }

    node->parent = parent;
    node->left   = NULL;
    node->right  = NULL;
    node->color  = ids_rbtree_color_red;
    *link        = node;

    Ids_RbTree_FixAdd(rbtree, node);

    return NULL;
}

inline void
Ids_RbTree_Del (struct ids_rbtree* rbtree, struct ids_rbtree_node* node)
{
    struct ids_rbtree_node* replacement;
    struct ids_rbtree_node* parent;
    struct ids_rbtree_node* removed       = node;
    enum ids_rbtree_color   removed_color = removed->color;

    if(node->left == NULL)
    {
        replacement = node->right;
        parent      = node->parent;
        Ids_RbTree_Replace(rbtree, node, node->right);
    }
    else if(node->right == NULL)
    {
        replacement = node->left;
        parent      = node->parent;
        Ids_RbTree_Replace(rbtree, node, node->left);
    }
    else
    {
        removed       = Ids_RbTree_NodeMin(node->right);
        removed_color = removed->color;
        replacement   = removed->right;

        if(removed->parent == node)
        {
            parent = removed;
            if(replacement != NULL)
                replacement->parent = removed;
        }
        else
        {
            parent = removed->parent;
            Ids_RbTree_Replace(rbtree, removed, removed->right);
            removed->right         = node->right;
            removed->right->parent = removed;
        }

        Ids_RbTree_Replace(rbtree, node, removed);
        removed->left         = node->left;
        removed->left->parent = removed;
        removed->color        = node->color;
    }

    if(removed_color == ids_rbtree_color_black)
        Ids_RbTree_FixDel(rbtree, replacement, parent);
}

inline void
Ids_RbTree_NBeginIt (const struct ids_rbtree_node* node, struct ids_rbtree_it* it)
{
    it->node = (struct ids_rbtree_node*)node;
}

inline void
Ids_RbTree_BeginIt (const struct ids_rbtree* rbtree, struct ids_rbtree_it* it)
{
    Ids_RbTree_NBeginIt(Ids_RbTree_Min(rbtree), it);
}

inline void
Ids_RbTree_RBeginIt (const struct ids_rbtree* rbtree, struct ids_rbtree_it* it)
{
    Ids_RbTree_NBeginIt(Ids_RbTree_Max(rbtree), it);
}

inline void
Ids_RbTree_ItFwd (struct ids_rbtree_it* it)
{
    it->node = Ids_RbTree_NodeNext(it->node);
}

inline void
Ids_RbTree_ItBack (struct ids_rbtree_it* it)
{
    it->node = Ids_RbTree_NodePrev(it->node);
}

inline int
Ids_RbTree_ItDone (const struct ids_rbtree_it* it)
{
    return it->node == NULL;
}

inline void
Ids_RbTree_BeginPostIt (const struct ids_rbtree* rbtree, struct ids_rbtree_post_it* it)
{
    it->node = Ids_RbTree_PostFirst(rbtree->root);
    it->next = it->node == NULL ? NULL : Ids_RbTree_PostNext(it->node);
}

inline void
Ids_RbTree_PostItFwd (struct ids_rbtree_post_it* it)
{
    it->node = it->next;
    it->next = it->node == NULL ? NULL : Ids_RbTree_PostNext(it->node);
}

inline int
Ids_RbTree_PostItDone (struct ids_rbtree_post_it* it)
{
    return it->node == NULL;
}
