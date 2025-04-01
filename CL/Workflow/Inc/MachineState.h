/*!
    MachineState.h
  
    Created on: 15.02.2017
        Author: Sambulov Dmitry
        e-mail: dmitry.sambulov@gmail.com
 */
#ifndef MACHINE_STATE_H_INCLUDED
#define MACHINE_STATE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*StateMachineHandler_t)(void *pxContext);

typedef struct {
  StateMachineHandler_t *pfStateEnter;
  StateMachineHandler_t *pfStateLoop;
  StateMachineHandler_t *pfStateExit;
  void *pxStateContext;
} MachineState_t;

typedef struct {
  __LinkedListObject__
  MachineState_t *pxStartState;
  MachineState_t *pxEndState;
  uint32_t eTrigger;
  StateMachineHandler_t *pfTransitionAction;
  void *pxActionContext;
} MachineStateTransition_t;

typedef struct {
  struct {
    MachineStateTransition_t pxTransitionsList;
    MachineState_t *pxCurrentState; /* Initially mast be set */
  } private;
  /* public */
  StateMachineHandler_t pfTransitionError;
} StateMachine_t;

void vStateMachineInit(StateMachine_t *pxMachine, MachineState_t *pxInitialState);
void vStateMachineAddTransition(StateMachine_t *pxMachine, MachineStateTransition_t *pxInitialState);
void vStateMachineWork(StateMachine_t *pxMachine);
void vStateMachineHandleEvent(StateMachine_t *pxMachine, uint32_t eTrigger);

#ifdef __cplusplus
}
#endif

#endif /* MACHINE_STATE_H_INCLUDED */
