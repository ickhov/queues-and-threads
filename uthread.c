#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

/* TODO Phase 2 */
#define THREAD_SIZE 32768
static ucontext_t ctx[THREAD_SIZE];
static uthread_t TID = 0;                // should be updated in uthread_create
static queue_t threads = NULL;            // should be created in uthread_create

typedef struct
{
    unsigned short TID;
    uthread_ctx_t *uctx;
    char *stack;
    /*
    STATE CODE
    0 = ready
    1 = running
    2 = has exited
    3 = blocked
    */
    int state;
    int joinedWithTID;
    int retval;
} TCB;                                     // thread control block

static TCB *previousTCB = NULL;
static TCB *currentTCB = NULL;

static int find_tcb_by_state(void *data, void *arg)
{
    TCB *a = (TCB*)data;
    int match = (int)(long)arg;

    if (a->state == match)
        return 1;

    return 0;
}

static int find_tcb_by_tid(void *data, void *arg)
{
    TCB *a = (TCB*)data;
    unsigned short match = *((unsigned short *)arg);

    if (a->TID == match)
        return 1;

    return 0;
}

TCB* getNextAvailableRunnableThread(void) {
    TCB *next = NULL;

    // find the next available "ready to run" thread
    queue_iterate(threads, find_tcb_by_state, (void*)0, (void **)&next);

    if (next != NULL) {
        queue_delete(threads, next);
    }

    return next;
}

void uthread_yield(void)
{
    //preempt_disable();

    previousTCB = currentTCB;
    currentTCB = getNextAvailableRunnableThread();

    //signal(SIGALRM, signal_alarm_handler);

    // update TCB for next thread
    if (currentTCB == NULL) {
        // no available thread ready to run so go back to main
        //uthread_ctx_switch(prev, &ctx[0]);
        currentTCB = previousTCB;
        return;
    } else {
        uthread_ctx_t *prev = NULL;
        uthread_ctx_t *cur = NULL;

        prev = previousTCB->uctx;

        // make sure state has not exited
        if (previousTCB->state != 2) {

            // Don't change blocked state into ready in yield
            if (previousTCB->state != 3) {
                // set next state to "ready to run"
                previousTCB->state = 0;
            }

            // put it back to the end of the queue to rerun later
            queue_enqueue(threads, previousTCB);
        }

        // set next state to "running"
        currentTCB->state = 1;

        cur = currentTCB->uctx;

        uthread_ctx_switch(prev, cur);
    }
}

uthread_t uthread_self(void)
{
    return currentTCB->TID;
}

int uthread_create(uthread_func_t func, void *arg)
{
    // increment TID for a new thread
    TID = TID + 1;

    // check for TID overflow
    if (TID == THREAD_SIZE) {
        return -1;
    }

    // create a new stack
    void *stack = uthread_ctx_alloc_stack();

    // check for memory allocation error
    if (stack == NULL) {
        return -1;
    }

    // initialize context object for new thread
    if (uthread_ctx_init(&ctx[TID], stack, func, arg) == -1) {
        return -1;
    }

    // if null, it's called for the first time
    if(threads == NULL) {
        threads = queue_create();
        preempt_start();

        // initialize main thread
        TCB *mainTCB = (TCB *) malloc(sizeof(TCB));
        mainTCB->TID = 0;
        mainTCB->uctx = &ctx[0];
        mainTCB->stack = NULL;
        mainTCB->state = 1; // ready to run
        mainTCB->joinedWithTID = -1; // default to no TID joining
        mainTCB->retval = -1; // default to unsuccessful return

        currentTCB = mainTCB;
    }

    TCB *tcb = (TCB *) malloc(sizeof(TCB));
    tcb->TID = TID;
    tcb->uctx = &ctx[TID];
    tcb->stack = stack;
    tcb->state = 0; // ready to run
    tcb->joinedWithTID = -1; // default to no TID joining
    tcb->retval = -1; // default to unsuccessful return

    queue_enqueue(threads, tcb);

    return TID;
}

void uthread_exit(int retval)
{
    // set next state to "has exited"
    currentTCB->state = 2;

    currentTCB->retval = retval;

    // check if this thread was joined
    if (currentTCB->joinedWithTID != -1) {
        // here we can collect the value since thread tid has exited
        TCB *parentTCB = NULL;

        // find the thread that thread tid joined with
        queue_iterate(threads, find_tcb_by_tid, (void*)&currentTCB->joinedWithTID, (void **)&parentTCB);

        // unblock the parent thread
        parentTCB->state = 0;

        /*
            1. Delete it from queue
            2. Rescheduled to run after all the runnable thread
        */
        queue_delete(threads, parentTCB);
        queue_enqueue(threads, parentTCB);
    }

    uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
    // the 'main' thread cannot be joined
    if (tid == 0) {
        return -1;
    }

    // tid cannot be the TID of the calling thread
    if (currentTCB->TID == tid) {
        return -1;
    }

    TCB *targetTCB = NULL;

    // find thread tid
    queue_iterate(threads, find_tcb_by_tid, (void*)&tid, (void **)&targetTCB);

    // TODO: create a while loop that consistently check the state of targetTCB
    // make sure that thread tid exists
    if (targetTCB != NULL) {

        // thread tid cannot be joined already
        if (targetTCB->joinedWithTID != -1) {
            return -1;
        }

        if (targetTCB->state != 2) {

            targetTCB->joinedWithTID = currentTCB->TID;
            currentTCB->state = 3;

            uthread_yield();
        }

        // set the retval
        if (retval != NULL) {
            *retval = targetTCB->retval;
        }

        // free thread tid resources
        uthread_ctx_destroy_stack(targetTCB->stack);
        free(targetTCB);

        if (currentTCB->TID == 0) {
            free(currentTCB);
            queue_destroy(threads);
            threads = NULL;
        }
    } else {
        // thread tid cannot be found
        return -1;
    }
    return 0;
}
