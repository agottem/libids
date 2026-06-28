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
    The hash data struct manages a hash of elements
 */


#ifndef IDS_HASH_H
#define IDS_HASH_H


#include <ids/err.h>
#include <ids/clist.h>


/*
    The node data to be placed in each hash element
 */
struct ids_hash_node
{
    struct ids_clist_node node;
};

/*
    The buckets which make up the hash data struct
 */
struct ids_hash_bkt
{
    struct ids_clist node_list;
};

/*
    The hash data struct itself
 */
struct ids_hash
{
    unsigned int         bkt_count;
    struct ids_hash_bkt* bkts;
};

/*
    The iterator struct used to iterate over an individual hash bucket

    Accessible members:
        current_node -- the current element node being visited
 */
struct ids_hash_bkt_it
{
    struct ids_hash_node* current_node;

    struct ids_clist_it   clist_it;
};

/*
    The iterator struct used to iterate over an entire hash

    Accessible members:
        current_node -- the current element node being visited
 */
struct ids_hash_it
{
    struct ids_hash_node*  current_node;

    unsigned int           current_bkt;
    struct ids_hash_bkt_it bkt_it;
};


/*
    The required type for a user-defined hash cmp function.  Return zero if the
    node matches the id

    For example:
        int
        MyCmp (void*                 id_to_cmp,
               struct ids_hash_node* cmp_node,
               void*                 user_data)
        {
            struct my_element* element;

            element = IDS_CONT_OF(cmp_node, struct my_element, node);

            return element->my_id != *(my_id_type*)id_to_cmp;
        }
 */
typedef int
ids_hash_cmp_t (void* id, struct ids_hash_node* node, void* user_data);


/*
     Initialize a hash.  The data struct will be empty upon initialization
 */
inline void
Ids_Hash_Init (unsigned int count, struct ids_hash_bkt* bkts, struct ids_hash* hash);

/*
     Allocate hash buckets and initialize a hash.  The data struct will be empty upon
     initialization
 */
inline enum ids_err
Ids_Hash_Create (unsigned int count, struct ids_hash* hash);

/*
     Cleanup a previously created hash
 */
inline void
Ids_Hash_Destroy (struct ids_hash* hash);

/*
    Reset a hash data struct to the initialized state
 */
inline void
Ids_Hash_Reset (struct ids_hash* hash);


/*
   Return the bucket which maps to the specified hash value
 */
inline struct ids_hash_bkt*
Ids_Hash_Bkt (unsigned int value_hash, struct ids_hash* hash);


/*
    Find a node in the hash.  Upon completion, the bucket searched for the node will be
    set and the found node returned.  If no node was found, NULL is returned
 */
inline struct ids_hash_node*
Ids_Hash_Find (unsigned int          value_hash,
               void*                 value,
               struct ids_hash*      hash,
               ids_hash_cmp_t*       cmp,
               void*                 user_data,
               struct ids_hash_bkt** searched_bkt);


/*
    Insert a node into the specified hash bucket
 */
inline void
Ids_Hash_Ins (struct ids_hash_node* node, struct ids_hash_bkt* bkt);

/*
    Add a node to the hash, specifying the hash value for the node
 */
inline void
Ids_Hash_Add (unsigned int value_hash, struct ids_hash_node* node, struct ids_hash* hash);

/*
    Delete the specified node from the hash
 */
inline void
Ids_Hash_Del (struct ids_hash_node* node);

/*
    Return non-zero if the hash bucket is empty
 */
inline int
Ids_Hash_BktEmpty (struct ids_hash_bkt* bkt);


/*
    Start an iteration of the specified hash bucket
 */
inline void
Ids_Hash_BeginBktIt (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it);

/*
    Iterate to the next node in the bucket
 */
inline void
Ids_Hash_BktItFwd (struct ids_hash_bkt* bkt, struct ids_hash_bkt_it* it);

/*
    Return non-zero if the iterator is at the end of the bucket
 */
inline int
Ids_Hash_BktItDone (struct ids_hash_bkt_it* it);


/*
    Start an unordered iteration of the hash
 */
inline void
Ids_Hash_BeginIt (struct ids_hash* hash, struct ids_hash_it* it);

/*
    Iterate to the next node in the hash
 */
inline void
Ids_Hash_ItFwd (struct ids_hash* hash, struct ids_hash_it* it);

/*
    Return non-zero if the iterator is at the end of the hash
 */
inline int
Ids_Hash_ItDone (struct ids_hash_it* it);


#include <ids/hash_inl.h>


#endif
