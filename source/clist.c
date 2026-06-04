#include <ids/clist.h>


extern void
IDS_CList_Init (struct ids_clist* clist);

extern void
IDS_CList_Reset (struct ids_clist* clist);


extern struct ids_clist_node*
IDS_CList_Head (struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_Tail (struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_End (struct ids_clist* clist);

extern int
IDS_CList_Empty (struct ids_clist* clist);


extern void
IDS_CList_Splice (
                  struct ids_clist_node* restrict start,
                  struct ids_clist_node* restrict end,
                  struct ids_clist_node* restrict dest
                 );

extern void
IDS_CList_Ins (
               struct ids_clist_node* restrict new_node,
               struct ids_clist_node* restrict existing_node
              );

extern void
IDS_CList_Del (struct ids_clist_node* node);

extern void
IDS_CList_PushH (struct ids_clist_node* node, struct ids_clist* clist);

extern struct ids_clist_node*
IDS_Clist_PopH (struct ids_clist* clist);

extern void
IDS_CList_PushT (struct ids_clist_node* node, struct ids_clist* clist);

extern struct ids_clist_node*
IDS_CList_PopT (struct ids_clist* clist);


extern void
IDS_CList_NBeginIt (struct ids_clist_node* node, struct ids_clist_it* it);

extern void
IDS_CList_BeginIt (struct ids_clist* clist, struct ids_clist_it* it);

extern void
IDS_CList_RBeginIt (struct ids_clist* clist, struct ids_clist_it* it);

extern void
IDS_CList_ItFwd (struct ids_clist_it* it);

extern void
IDS_CList_ItBack (struct ids_clist_it* it);

extern int
IDS_CList_ItDone (struct ids_clist* clist, struct ids_clist_it* it);
