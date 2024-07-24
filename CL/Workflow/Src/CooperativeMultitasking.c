#include "CodeLib.h"
#include "LinkedList.h"

typedef struct {
  uint8_t bCancel;
  void *pxArg;
} WorkContext_t;

typedef struct {
  __LinkedListObject__
  CoroutineHandler_t handler;
  WorkContext_t xContext;
} CoroutinePrivate_t;

_Static_assert(sizeof(CoroutinePrivate_t) == sizeof(Coroutine_t), "In CooperativeMultitasking.h data structure size of Coroutine_t doesn't match, check CO_ROUTINE_DESC_SIZE");

typedef struct {
  uint8_t bForceCancel;
  uint32_t bLeft;
} SchedulerArg_t;

static LinkedList_t workList = libNULL;

void vCoroutineAdd(Coroutine_t *pcCoRBuffer, CoroutineHandler_t pfHandler, void* pxArg) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pcCoRBuffer;
  if((worker != libNULL) && (pfHandler != libNULL)) {
    worker->handler = pfHandler;
    worker->xContext.bCancel = 0;
    worker->xContext.pxArg = pxArg;
    vLinkedListInsert(&workList, LinkedListItem(worker), libNULL);
  }
}

void vCoroutineSetContext(Coroutine_t *pxCoR, void* pxArg) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if(worker != libNULL) {
    worker->xContext.pxArg = pxArg;
  }
}

void vCoroutineCancel(Coroutine_t *pxCoR) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if(worker != libNULL) {
    worker->xContext.bCancel = 1;
  }
}

void vCoroutineTerminate(Coroutine_t *pxCoR) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if(worker != libNULL) {
    vLinkedListUnlink(LinkedListItem(worker));
  }
}

static void _vCoroutineRun(LinkedListItem_t *desc, void *pxArg) {
  CoroutinePrivate_t *wrk = LinkedListGetObject(CoroutinePrivate_t, desc);
  SchedulerArg_t *arg = (SchedulerArg_t *)pxArg;
  if(wrk->handler != libNULL) {
    uint8_t bCancel = wrk->xContext.bCancel || arg->bForceCancel;
    if(wrk->handler((Coroutine_t *)wrk, bCancel, wrk->xContext.pxArg)) {
      vLinkedListUnlink(desc);
    }
    else {
      arg->bLeft++;
    }
  }
}

uint32_t ulCooperativeScheduler(uint8_t bCancelAll) {
  SchedulerArg_t arg = {.bForceCancel = bCancelAll, .bLeft = 0};
  ulLinkedListDoForeach(workList, &_vCoroutineRun, (void *)&arg);
  return arg.bLeft;
}

void coroutine_add(coroutine_t *cor_buf, coroutine_handler_t handler, void *arg)
                                                            __attribute__ ((alias ("vCoroutineAdd")));
void coroutine_set_context(coroutine_t *cor, void* arg) __attribute__ ((alias ("vCoroutineSetContext")));
void coroutine_cancel(coroutine_t *cor)                 __attribute__ ((alias ("vCoroutineCancel")));
void coroutine_terminate(coroutine_t *cor)              __attribute__ ((alias ("vCoroutineTerminate")));
uint32_t cooperative_scheduler(uint8_t cancel_all)          __attribute__ ((alias ("ulCooperativeScheduler")));
