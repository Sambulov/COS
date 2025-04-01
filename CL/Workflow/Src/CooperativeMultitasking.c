#include "CodeLib.h"

typedef struct {
  __LinkedListObject__
  CoroutineHandler_t handler;
  void *pxArg;
} CoroutinePrivate_t;

LIB_ASSERRT_STRUCTURE_CAST(CoroutinePrivate_t, Coroutine_t, CO_ROUTINE_DESC_SIZE, CooperativeMultitasking.h);

typedef struct {
  uint8_t bCancel;
  uint32_t bLeft;
} SchedulerArg_t;

static LinkedList_t xTasksRun = libNULL;
static LinkedList_t xTasksCancel = libNULL;
//static LinkedList_t xTasksSuspend = libNULL;

void vCoroutineAdd(Coroutine_t *pcCoRBuffer, CoroutineHandler_t pfHandler, void* pxArg) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pcCoRBuffer;
  if((worker != libNULL) && (pfHandler != libNULL)) {
    worker->handler = pfHandler;
    worker->pxArg = pxArg;
    vLinkedListInsert(&xTasksRun, LinkedListItem(worker), libNULL);
  }
}

void vCoroutineSetContext(Coroutine_t *pxCoR, void* pxArg) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if(worker != libNULL) {
    worker->pxArg = pxArg;
  }
}

void vCoroutineCancel(Coroutine_t *pxCoR) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if((worker != libNULL) && (bLinkedListContains(xTasksRun, LinkedListItem(worker)))) {
    vLinkedListInsert(&xTasksCancel, LinkedListItem(worker), libNULL);
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
    if(wrk->handler((Coroutine_t *)wrk, arg->bCancel, wrk->pxArg)) {
      vLinkedListUnlink(desc);
    }
    else {
      arg->bLeft++;
    }
  }
}

uint32_t ulCooperativeScheduler(uint8_t bCancelAll) {
  SchedulerArg_t arg = {.bCancel = bCancelAll, .bLeft = 0};
  ulLinkedListDoForeach(xTasksRun, &_vCoroutineRun, (void *)&arg);
  arg.bCancel = cl_true;
  ulLinkedListDoForeach(xTasksCancel, &_vCoroutineRun, (void *)&arg);
  return arg.bLeft;
}

CoroutineState_t eCoroutineState(Coroutine_t *pxCoR) {
  CoroutinePrivate_t *worker = (CoroutinePrivate_t *)pxCoR;
  if(worker != libNULL) {
    if(bLinkedListContains(xTasksRun, LinkedListItem(worker))) {
      return CO_ROUTINE_RUN;
    } 
    else if(bLinkedListContains(xTasksCancel, LinkedListItem(worker))) {
      return CO_ROUTINE_CANCELATION;
    }
  }
  return CO_ROUTINE_UNKNOWN;
}


void coroutine_add(coroutine_t *cor_buf, coroutine_handler_t handler, void *arg)
                                                            __attribute__ ((alias ("vCoroutineAdd")));
void coroutine_set_context(coroutine_t *cor, void* arg)     __attribute__ ((alias ("vCoroutineSetContext")));
void coroutine_cancel(coroutine_t *cor)                     __attribute__ ((alias ("vCoroutineCancel")));
void coroutine_terminate(coroutine_t *cor)                  __attribute__ ((alias ("vCoroutineTerminate")));
coroutine_state_t coroutine_state(coroutine_t *cor)         __attribute__ ((alias ("eCoroutineState")));
uint32_t cooperative_scheduler(uint8_t cancel_all)          __attribute__ ((alias ("ulCooperativeScheduler")));
