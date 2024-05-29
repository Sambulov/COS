#ifndef COOPERATIVE_MT_H_INCLUDED
#define COOPERATIVE_MT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define CO_ROUTINE_DESC_SIZE  28

typedef struct {
  CL_PRIVATE(CO_ROUTINE_DESC_SIZE);
} CoroutineDescStatic_t;

typedef CoroutineDescStatic_t *CoroutineDesc_t;

typedef uint8_t (*CoroutineHandler_t)(CoroutineDesc_t pxThis, uint8_t bCancel, void *pxArg);

void vCoroutineAddStatic(CoroutineDescStatic_t *pcCoRBuffer, CoroutineHandler_t pfHandler, void *pxArg);
void vCoroutineSetContext(CoroutineDesc_t pxCoR, void* pxArg);
void vCoroutineCancel(CoroutineDesc_t pxCoR);
void vCoroutineTerminate(CoroutineDesc_t pxCoR);

uint32_t ulCooperativeScheduler(uint8_t bCancelAll);

/*!
  Snake notation
*/

typedef CoroutineDescStatic_t coroutine_desc_static_t;
typedef CoroutineDescStatic_t *coroutine_desc_t;
typedef CoroutineHandler_t coroutine_handler_t;

void coroutine_add_static(coroutine_desc_static_t *cor_buf, coroutine_handler_t handler, void *arg);
void coroutine_set_context(coroutine_desc_t cor, void* arg);
void coroutine_cancel(coroutine_desc_t cor);
void coroutine_terminate(coroutine_desc_t cor);

uint32_t cooperative_scheduler(uint8_t cancel_all);

#ifdef __cplusplus
}
#endif

#endif /* COOPERATIVE_MT_H_INCLUDED */ 
