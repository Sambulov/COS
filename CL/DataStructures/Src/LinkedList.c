#include "CodeLib.h"
#include "LibObjValidation.h"

// typedef struct LL_ITEM_T {
// 	uint32_t validation;
// 	struct LL_ITEM_T *next;
// 	struct LL_ITEM_T *prev;
// 	struct LL_ITEM_T **listPointer;
// } __LinkedListItem_t;

// _Static_assert(sizeof(__LinkedListItem_t) == sizeof(LinkedListItem_t), "In LinkedList.h data structure size of LinkedListItem_t doesn't match, check LL_ITEM_SIZE");

#define _bIsValidItem(it)         ((it == libNULL) || (it->validation == LINKED_LIST_VALIDATION))

static void _vLinkBefore(LinkedListItem_t *pxCurrent, LinkedListItem_t *pxNew) {
	if (pxCurrent != libNULL && (pxNew != libNULL) && (pxCurrent != pxNew)) {
		LinkedListItem_t* beforeCurrent = pxCurrent->prev;
		pxCurrent->prev = pxNew;
		pxNew->next = pxCurrent;
		pxNew->prev = beforeCurrent;
		beforeCurrent->next = pxNew;
		pxNew->listPointer = pxCurrent->listPointer;
	}
}

void vLinkedListUnlink(LinkedListItem_t *pxItem) {
	if (pxItem != libNULL) {
		if (_bIsValidItem(pxItem) && (pxItem->listPointer != libNULL)) {
			if (*(pxItem->listPointer) == pxItem) {
				if(pxItem->next == pxItem) {
					*pxItem->listPointer = libNULL;
				}
				else {
					*pxItem->listPointer = pxItem->next;
				}
			}
			LinkedListItem_t* prevItem = pxItem->prev;
			LinkedListItem_t* nextItem = pxItem->next;
			prevItem->next = pxItem->next;
			nextItem->prev = pxItem->prev;
	    }
		pxItem->validation = LINKED_LIST_VALIDATION;
		pxItem->next = pxItem;
		pxItem->prev = pxItem;
		pxItem->listPointer = libNULL;
	}
}

void vLinkedListInsert(LinkedList_t *ppxLinkedList, LinkedListItem_t *pxItem, ListItemComparer_t pfComparer) {
	if ((pxItem == libNULL) || (ppxLinkedList == libNULL)) {
		return;
	}
	vLinkedListUnlink(pxItem);
	LinkedListItem_t *current = *ppxLinkedList;
	if(_bIsValidItem(current)) {
		pxItem->listPointer = ppxLinkedList;
		if ((pfComparer != libNULL) && (current != libNULL)) {
			while (pfComparer(pxItem, current) > 0) {
				current = current->next;
				if (current == *ppxLinkedList) {
					break;
				}
			}
		}
		_vLinkBefore(current, pxItem);
		*ppxLinkedList = pxItem;
	}
}

void vLinkedListInsertLast(LinkedList_t *ppxLinkedList, LinkedListItem_t *pxItem) {
	if(ppxLinkedList != libNULL) {
    	vLinkedListUnlink(pxItem);
		LinkedList_t pxLinkedList = *ppxLinkedList;
		vLinkedListInsert(ppxLinkedList, pxItem, libNULL);
		if (pxLinkedList != libNULL) {
			*ppxLinkedList = pxLinkedList;
		}
	}
}

static LinkedListItem_t *_pxLinkedListFind(LinkedListItem_t *pxCurrent, LinkedListMatch_t pfMatch, void *pxSearchArgs, uint8_t bFirst, uint8_t bOverlap) {
	if ((pxCurrent != libNULL) && (pfMatch != libNULL) && _bIsValidItem(pxCurrent)) {
		LinkedList_t *start = pxCurrent->listPointer;
		if ((start == libNULL) || (bOverlap && !bFirst)) {
			start = &pxCurrent;
		}
		if(bFirst) {
			pxCurrent = *start;
		}
		do {
			if (!bFirst) {
				pxCurrent = pxCurrent->next;
				if(pxCurrent == *start) {
					break;
				}
			}
			if (pfMatch(pxCurrent, pxSearchArgs)) {
				return pxCurrent;
			}
			if (bFirst) {
				pxCurrent = pxCurrent->next;
			}
		} while (pxCurrent != *start);
	}
	return libNULL;
}

LinkedListItem_t *pxLinkedListFindFirst(LinkedList_t pxLinkedList, LinkedListMatch_t pfMatch, void *pxSearchArgs) {
	return _pxLinkedListFind(pxLinkedList, pfMatch, pxSearchArgs, 1, 0);
}

LinkedListItem_t *pxLinkedListFindNextOverlap(LinkedListItem_t *pxCurrent, LinkedListMatch_t pfMatch, void *pxSearchArgs) {
	return _pxLinkedListFind(pxCurrent, pfMatch, pxSearchArgs, 0, 1);
}

LinkedListItem_t *pxLinkedListFindNextNoOverlap(LinkedListItem_t *pxCurrent, LinkedListMatch_t pfMatch, void *pxSearchArgs) {
	return _pxLinkedListFind(pxCurrent, pfMatch, pxSearchArgs, 0, 0);
}

void vLinkedListDoForeach(LinkedList_t pxList, LinkedListAction_t fAction, void *pxArg) {
	if ((fAction != libNULL) && _bIsValidItem(pxList)) {
		LinkedList_t *pxRoot = pxList->listPointer;
		if(pxRoot != libNULL) {
			LinkedList_t currentItem = *pxRoot;
			LinkedList_t nextItem;
			do {
				nextItem = currentItem->next;
				fAction(currentItem, pxArg);
				currentItem = nextItem;
			} while((*pxRoot != libNULL) && (currentItem != *pxRoot) && (currentItem->listPointer == pxRoot));
		}
		else /* if it is linked list single item */{
			fAction(pxList, pxArg);
		}
	}
}

uint32_t ulLinkedListCount(LinkedList_t pxList, LinkedListMatch_t pfMatch, void *pxSearchArgs) {
	uint32_t count = 0;
	if ((pxList != libNULL) && _bIsValidItem(pxList)) {
		LinkedListItem_t *start = pxList;
		do {
			if ((pfMatch == libNULL) || pfMatch(pxList, pxSearchArgs)) {
				count++;
			}
			pxList = pxList->next;
		} while (pxList != start);
	}
	return count;
}

uint8_t bLinkedListContains(LinkedList_t pxList, LinkedListItem_t *pxItem) {
	return (pxList != libNULL) && 
		_bIsValidItem(pxList) && 
		(pxItem != libNULL) && 
		_bIsValidItem(pxItem) && 
		(pxList->listPointer == pxItem->listPointer);	
}

linked_list_item_t *linked_list_find_first(linked_list_t linked_list, linked_list_match_t match_fn, void *search_args)\
                                                    __attribute__ ((alias ("pxLinkedListFindFirst")));

linked_list_item_t *linked_list_find_next_overlap(linked_list_item_t *current, linked_list_match_t match_fn, void *search_args)\
                                                    __attribute__ ((alias ("pxLinkedListFindNextOverlap")));

linked_list_item_t *linked_list_find_next_no_overlap(linked_list_item_t *current, linked_list_match_t match_fn, void *search_args)\
                                                    __attribute__ ((alias ("pxLinkedListFindNextNoOverlap")));

void linked_list_do_foreach(linked_list_t linked_list, linked_list_action_t action_fn, void *arg)\
                                                    __attribute__ ((alias ("vLinkedListDoForeach")));

void linked_list_insert(linked_list_t *linked_list_ptr, linked_list_item_t *item, list_item_comparer_t comparer_fn)\
                                                    __attribute__ ((alias ("vLinkedListInsert")));

void linked_list_insert_last(linked_list_t *linked_list_ptr, linked_list_item_t *item)\
                                                    __attribute__ ((alias ("vLinkedListInsertLast")));

void linked_list_unlink(linked_list_item_t *item)   __attribute__ ((alias ("vLinkedListUnlink")));

uint32_t linked_list_count(linked_list_t linked_list, linked_list_match_t match_fn, void *search_args)\
                                                    __attribute__ ((alias ("ulLinkedListCount")));

uint8_t linked_list_contains(linked_list_t list, linked_list_item_t *item)\
                                                    __attribute__ ((alias ("bLinkedListContains")));
