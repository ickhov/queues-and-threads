# ECS150P2

**Uthread API**

```c
typedef struct{
    unsigned short TID;
    uthread_ctx_t *uctx;
    char *stack;
    int state;
    int joinedWithTID;
    int retval;
} TCB;
```

```c
void uthread_yield(void)
```
We save the current TCB as the previous TCB and find the next available running
thread and save it's TCB value to the current TCB. If there is no available
running thread, we go back to main. Else, if there is a running thread then we
make sure that the previous state has not exited and and not blocked set the
previous state as "ready to run" and add it to the queue of running threads. If
the state has not exited and is blocked, we add it to the queue of blocked
threads. Then we set the next state to "running" and perform a context switch
between the previously running thread and the current running thread. 

```c
static int find_tcb_by_tid(void *data, void *arg)
```
This function is used as a parameter for the queue_iterate function so that we
can search the queue for a TCB, given a tid. 

```c
int uthread_create(uthread_func_t func, void *arg)
```
In our uthread_create function, before creating a new thread with a new TCB
struct, we check to see if it is the first thread being created. If it is the
first thread created, we will first create a TCB struct for the already running
thread in main and set it's stack to NULL and it's TID to 0. The main thread
does not need to be added to the queue here, it is added to the queue when
uthread_yield is called. We create two different queues, one for the running
threads, and one for the blocked threads. Then, we add the thread to the queue
and return the Thread ID. 

```c
void uthread_exit(int retval)
```
We first set the next state to "has exited", and save the return value to the
current TCB. Then we check to see if the thread has a child. If the thread has a
child, it cannot exit until it's child has completed. If the thread has no
child, we collect the value since thread tid has exited. We use the
queue_iterate function to find the parent with a search by tid with the function
find_tcb_by_tid. Then, we unblock the parent thread, delete it from the blocked
threads queue and reschedule it to run after all the currently running threads
by adding it back to the running threads queue. The function ends by calling
uthread_yield. 

```c
int uthread_join(uthread_t tid, int *retval)
```
In order to find the TCB for the thread to be joined, we use queue_iterate with
the find_tcb_by_tid function as a parameter to search the queue for the TCB
using a tid. One the targetTCB is found, we make sure that the thread tid exists
and can be joined. We then block the parent TCB and denote that the child has a
parent by setting the joinedWithTID value of the child to the parent's TID. This
is done so that when the child exits, it can find the parent and unblock it. 


**Preempt API**

```c
void sigvtalrm_handler(int signum)
```
The alarm handler function will call *uthread_yield* every time that there is a
timer interrupt. 

```c
void preempt_start(void)
```
We have a *sigset_t sig_mask* global variable that is used to block and unblock
the signal. We set up sigaction with the sigvtalrm_handler function and set the
flags to 0. We also have a timer *timer.it_interval.tv_usec* is used to set a
timer interrupt every 10 milliseconds and *timer.it_value.tv_usec* to repeat
every 10 milliseconds. 


**Queue API**

```c
queue_t queue_create(void)
```
In this function, we create a queue struct pointer, that will contain a double
pointer of any type, an index value for the head to handle dequeue, and index
value to handle enqueue, and int holding the size of the queue.

```c
int queue_destroy(queue_t queue)
```
This function first checks to see if the queue is NULL or empty before freeing
the memory for the empty queue struct.

```c
int queue_enqueue(queue_t queue, void *data)
```
This function will add new items into the queue by incrementing five spaces for
everytime that it needs to realloc and allocates more space for the data in the
queue.

```c
int queue_dequeue(queue_t queue, void **data)
```
This function checks to see if the queue is empty before continuing. It then
will assign the oldest item to data and then remove it from the queue. After
doing this, the head is updated to the next oldest item.

```c
int queue_delete(queue_t queue, void *data)
```
The queue first checks to see if the queue or data is NULL before continuing.
The queue loops through to find the item to delete, if the item is found, every
item after that position is shifted back once and the last element is set to
NULL. The tail is then decremented once in order to account for the removal.

```c
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
```
Our queue iterate function searches through the queue to find an item specified
by the func argument. It checks to see if the function returns 1, and if it
does, it will set *data to the current item found. 