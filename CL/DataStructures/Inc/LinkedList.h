/*!
    LinkedList.h
  
    Created on: 15.02.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */
#ifndef LINKED_LIST_H_INCLUDED
#define LINKED_LIST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//#define LL_ITEM_SIZE    16

//typedef struct LL_ITEM_T {
//  uint8_t __prv[LL_ITEM_SIZE];
//} LinkedListItem_t;

/*!
  Camel case notation
*/

typedef struct LL_ITEM_T {
	uint32_t validation;
	struct LL_ITEM_T *next;
	struct LL_ITEM_T *prev;
	struct LL_ITEM_T **listPointer;
} LinkedListItem_t;

typedef LinkedListItem_t* LinkedList_t;
typedef int32_t (*ListItemComparer_t)(LinkedListItem_t *, LinkedListItem_t *);
typedef void (*LinkedListAction_t)(LinkedListItem_t*, void *pxArg);
typedef uint8_t (*LinkedListMatch_t)(LinkedListItem_t*, void *pxArg);

LinkedListItem_t *pxLinkedListFindFirst(LinkedList_t pxList, LinkedListMatch_t pfMatch, void *pxSearchArgs);
LinkedListItem_t *pxLinkedListFindNextOverlap(LinkedListItem_t *pxCurrent, LinkedListMatch_t pfMatch, void *pxSearchArgs);
LinkedListItem_t *pxLinkedListFindNextNoOverlap(LinkedListItem_t *pxCurrent, LinkedListMatch_t pfMatch, void *pxSearchArgs);
void vLinkedListDoForeach(LinkedList_t pxList, LinkedListAction_t fAction, void *pxArg);
void vLinkedListInsert(LinkedList_t *ppxList, LinkedListItem_t *pxItem, ListItemComparer_t pfComparer);
void vLinkedListInsertLast(LinkedList_t *ppxLinkedList, LinkedListItem_t *pxItem);
void vLinkedListUnlink(LinkedListItem_t *pxItem);
uint32_t ulLinkedListCount(LinkedList_t pxList, LinkedListMatch_t pfMatch, void *pxSearchArgs);
void vLinkedListClear(LinkedList_t *pxList);
uint8_t bLinkedListContains(LinkedList_t pxList, LinkedListItem_t *pxItem);

/*!
  Snake notation
*/

typedef LinkedListItem_t linked_list_item_t;
typedef LinkedListItem_t* linked_list_t;
typedef ListItemComparer_t list_item_comparer_t;
typedef LinkedListAction_t linked_list_action_t;
typedef LinkedListMatch_t linked_list_match_t;

linked_list_item_t *linked_list_find_first(linked_list_t linked_list, linked_list_match_t match_fn, void *search_args);
linked_list_item_t *linked_list_find_next_overlap(linked_list_item_t *current, linked_list_match_t match_fn, void *search_args);
linked_list_item_t *linked_list_find_next_no_overlap(linked_list_item_t *current, linked_list_match_t match_fn, void *search_args);
void linked_list_do_foreach(linked_list_t linked_list, linked_list_action_t action_fn, void *arg);
void linked_list_insert(linked_list_t *linked_list_ptr, linked_list_item_t *item, list_item_comparer_t comparer_fn);
void linked_list_insert_last(linked_list_t *linked_list_ptr, linked_list_item_t *item);
void linked_list_unlink(linked_list_item_t *item);
uint32_t linked_list_count(linked_list_t linked_list, linked_list_match_t match_fn, void *search_args);
void linked_list_clear(LinkedList_t *linked_list_ptr);
uint8_t linked_list_contains(linked_list_t linked_list, linked_list_item_t *item);

#ifdef __cplusplus
}
#endif

#endif /* LINKED_LIST_H_INCLUDED */
