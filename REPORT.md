# ECS150P2

## Queue API

```c
struct queue {
    void **item;    // pointers to all the items in the queue
    int head;       // beginning of queue
    int tail;       // ending of queue
    int size;       // total size of queue we allocated in memory
};
```

```c
queue_t queue_create(void)
```
We allocated memory for a *queue* pointer. Then, we set *item* to NULL since
we're not adding any item, set *head*, *tail*, and *size* to 0, and return the
*queue* pointer.

```c
int queue_destroy(queue_t queue)
```
We check to make sure *queue* is not null and empty. If either one of them is
false, then we return -1. Else, we deallocate the memory for *item* pointer
array along with the queue. We don't deallocate the memory for the pointers
inside the *item* pointer array because they are handled in *uthread_exit()* and
*uthread_join()* function.

```c
int queue_enqueue(queue_t queue, void *data)
```
We check to make sure *queue* and *data* are not NULL. If either one of them is
NULL, then we return -1. Else, we check if there's enough room in the *item*
pointer array to store a new *data* pointer. If not, we reallocate the *item*
pointer array with 5 more spaces than before. We chose this approach to minimize
the amount of realloc calls and memory we have to make for the *item* pointer
array itself. After doing these checks, we add the *data* pointer to the end of
the *item* pointer array and increment *tail* by 1 to track the end of the
queue.

```c
int queue_dequeue(queue_t queue, void **data)
```
We check to make sure *queue* is not NULL and has data in it. If either one of
them is false, then we return -1. Else, we dereference *data* and set it equal
to the first item in the queue using the *head* variable as the index. Then, we
set the first item in the queue to NULL and increment *head* by 1 so we don't
access that index again. Before we return data, we also check to make sure the
value of the dereference *data* is not NULL. We believe that this case would
rarely happen because we don't enqueue NULL data unless we accidentally
deferenced that data somewhere in uthread.

```c
int queue_delete(queue_t queue, void *data)
```
We check to make sure *queue* is not NULL and has data in it. If either one of
these conditions is false, then we return -1. Else, we loop through the queue to
find the first instance of the *data*. Once we find the *data*, we starts a
nested for-loop that essentially shifts every item in queue left by one index,
starting from the index of the found data and return 0. After, we set the last
index to NULL because we don't want *queue->item[index - 2]* equal to
*queue->item[index - 1]*. This will remove the first instance of the data from
the queue and make sure there's no NULL item in between the queue. If the outer
for-loop finished running, then we never found the data in the queue and return
-1.

```c
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
```
We check to make sure *queue* and *func* pointer are not NULL. If either one of
these conditions is false, then we return -1. Else, we loop through the queue
and call the function on each element in the queue. Since *func* can return a
value, we check in an if-statement if it returns 1. If it does and *data* is not
NULL, we dereference *data*, set it equal to the current element specified by
the index, and break out of the loop so we can return 0.

```c
int queue_length(queue_t queue)
```
We return the actual length of the queue doing *tail* (end) minus *head*
(begin). We didn't use the *size* variable because it's used to realloc more
space for the queue, not track the actual size of the queue.

## Testing Queue
We test our queue using test_queue.c in which we created individual functions to
test each implementation in queue. The file tests for the failure and success
cases we described for each function above.

## Uthread API

```c
typedef struct{
    unsigned short TID;     // thread ID
    uthread_ctx_t *uctx;    // thread context
    char *stack;            // track the thread statck
    int state;  // 0 = ready, 1 = running, 2 = has exited, 3 = blocked
    int joinedWithTID;      // thread ID of the child TCB
    int retval;             // return value of thread
} TCB;
```

```c
void uthread_yield(void)
```
We save the previous thread and then set the current thread to the next
available thread from the queue. If the current thread is NULL, then we return.
If it's not NULL, then we check if the previous thread has already exited. If
so, then we don't add it back to the queue. If it hasn't exited, then we check
the TCB state and either add it to the *ready* or *blocked* threads. Then, we
set the current thread's status to *running* and perform a context switch.

```c
uthread_t uthread_self(void)
```
We return the TID of the current running thread.

```c
int uthread_create(uthread_func_t func, void *arg)
```
We do multiple checks to make sure there's no thread overflow and there's enough
memory for malloc before doing any operation. Then we create the *ready* and
*blocked* queues if it was called for the first time. Then, we create a *TCB*
pointer to represent a new thread, add it to the queue and return its thread ID.

```c
void uthread_exit(int retval)
```
We check to see if the current thread has a parent. If the current thread has a
parent, we search for the parent thread with the *queue_iterate()* function,
unblock it, remove it from the blocked threads queue and add it to the running
threads queue. Then, regardless of whether or not the current thread has a
parent, we call *uthread_yield()*.

```c
int uthread_join(uthread_t tid, int *retval)
```
We check to make sure that thread tid is not the main thread and the current
thread. Then, we find thread tid in the *ready* queue using *queue_iterate()*.
If we can't find it, then we return -1. Else, we check to make sure it hasn't
alreayd been joined with the current thread before proceeding. Then, if the
thread tid has not exited yet, then we block the current thread and do a context
switch. After we return to the current thread, we deallocate memory related to
the child thread. If the current thread happens to be the main thread, then we
do context switch until we successfully destroy the *ready* queue so we can
deallocate memory from the main thread *TCB* and destroy *blocked* queue.

## Testing Uthread
We test our uthread using 3 seperate files: uthread_hello (tests
*uthread_create()*), uthread_yield, and uthread_join. 

NOTE: In order to test uthread, you will need to set *allow_preempt* static
variable to 0 at the top of *uthread.c* file for deterministic output.

## Preempt API

```c
void sigvtalrm_handler(int signum)
```
This function is an alarm handler that will call *uthread_yield* every time that
there is a timer interrupt. 

```c
void preempt_disable(void)
```
This function blocks the *SIGVTALRM* signal by setting the mask to *SIG_BLOCK*.

```c
void preempt_enable(void)
```
This function unblocks the *SIGVTALRM* signal by setting the mask to
*SIG_UNBLOCK*.

```c
void preempt_start(void)
```
We set up the *sigset_t sig_mask* global variable that is used to block and
unblock the *SIGVTALRM* signal. We set up sigaction with the sigvtalrm_handler
function. We also set up a timer that would run for 10 milliseconds and repeat
every 10 milliseconds.

## Testing Preempt
We test our preempt using test_preempt.c in which we created 3 threads with no
yield functions. The output is unpredictable, but all the threads print its
respective *printf* statement which implies that preempt is working. 

NOTE: In order to test preempt you will need to set *allow_preempt* static
variable to 1 at the top of *uthread.c* file.

# Preempt usage in uthread
We disable preempt every time we call a queue-related function and enable it
after those calls. We also disable preempt at the start of yield and enable it
after we call yield since that thread will resume running at one point. We don't
enable preempt after yield in exit since we will never run that thread again.

## Sources we used to write preempt
We used the example from this
[website](http://www.informit.com/articles/article.aspx?p=23618&seqNum=14) to
help us write *preempt_start()*.