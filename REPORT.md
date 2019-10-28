# ECS150P2

## Queue API

We created a queue struct that contains pointers to all the items in the queue, the head and tail of the queue, and the size of the queue.
```c
struct queue {
    void **item;
    int head;
    int tail;
    int size;
};
```

```c
queue_t queue_create(void)
```
In this function, we allocated memory for a *queue struct* pointer. We check to make sure the pointer is not NULL after malloc in case it failed due to no more memory. Then, we set *item* to NULL since we're not adding any item yet and set *head*, *tail*, and *size* to 0. Then, we return the *queue struct* pointer.

```c
int queue_destroy(queue_t queue)
```
In this function, before we perform any operation, we check to make sure the *queue* is not null and it's empty. If either one of these conditions is false, then we return -1 immediately. If both are true, we deallocate the memory for the *item* pointer array variable in the queue and the queue itself. We don't deallocate the memory for the pointers inside the *item* pointer array because they are handled in *uthread_exit()* and *uthread_join()* function.

```c
int queue_enqueue(queue_t queue, void *data)
```
In this function, before we perform any operation, we check to make sure the *queue* and the *data* are not NULL. If either one of them are NULL, then we return -1 immediately. If both are true, we continue to check to make sure that there's enough room in the *item* pointer array to store a new *data* pointer. If not, we reallocate the *item* pointer array with 5 more spaces than before. We chose this approach to minimize the amount of time we have to call realloc and to minimize the amount of memory we have to make for the *item* pointer array itself. As always, after calling realloc, we check to make sure it was successful. After doing these checks, we add the *data* pointer to the end of the *item* pointer array and increment *tail* by 1 when we need to enqueue again.

```c
int queue_dequeue(queue_t queue, void **data)
```
In this function, before we perform any operation, we check to make sure the *queue* is not NULL and has data in it. If either one of these conditions is false, then we return -1 immediately. If both are true, we dereference *data* and set it equal to the first item in the queue using the *head* variable as the index. Then, we set the first data in the queue to NULL and increment *head* by 1 so we don't access that index again. Before we return data, we also check to make sure the value of the dereference *data* is not NULL. We believe that this case would rarely happen because we don't enqueue NULL data. The only time it would happen is when we accidentally deallocate the memory for that data in uthread due to a bug in our algorithm.

```c
int queue_delete(queue_t queue, void *data)
```
In this function, before we perform any operation, we check to make sure the *queue* is not NULL and has data in it. If either one of these conditions is false, then we return -1 immediately. If both are true, 
then we loop through the queue to find the first instance of the *data*. Once we find the *data*, we use the index and starts a nested for-loop that essentially shifts every item in queue left by one index and return 0. For example, if the data is in index 2, we would set *queue->item[2] = queue->item[3]* and so on. We finally set the last index to NULL because we don't want *queue->item[index - 2]* equal to *queue->item[index - 1]*. This will remove the first instance of the data from the queue and making sure there's no NULL item in between the queue. If the outer for-loop finished running without invoking the inner for-loop, then we never found the data in the queue and return -1.

```c
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
```
In this function, before we perform any operation, we check to make sure the *queue* and *func* pointer are not NULL. If either one of these conditions is false, then we return -1 immediately. If both are true, we loop through the queue and call the function on each element in the queue. Since *func* can return a value, we check in an if-statement if it returns 1. If it does and *data* is not NULL, we dereference *data*, set it equal to the current element specified by the index, and break out of the loop so we can return 0.

```c
int queue_length(queue_t queue)
```
In this function, we just return the actual length of the queue doing *tail* minus *head*. We used these variables to get the length rather the *size* variable because we are used them to track the beginning and end of the queue while *size* is used to realloc more space for the queue.

## Testing Queue
We test our queue using test_queue.c. Inside this file, we created individual functions to test each implementation in queue. The test files essentially tests for failure and success cases we described for each function above.


## Uthread API

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

## Testing Uthread
... NOTE: In order to test queue you will need to set *allow_preempt* static variable to 0 at the top of *uthread.c* file. This will prevent preempt from running in uthread and allow for deterministic behavior.


## Preempt API

```c
void sigvtalrm_handler(int signum)
```
This function is an alarm handler that will call *uthread_yield* every time that there is a timer interrupt. 

```c
void preempt_disable(void)
```
This function blocks the *SIGVTALRM* signal by setting the mask to *SIG_BLOCK*.

```c
void preempt_enable(void)
```
This function unblocks the *SIGVTALRM* signal by setting the mask to *SIG_UNBLOCK*.

```c
void preempt_start(void)
```
In this function, we set up the *sigset_t sig_mask* global variable that is used to block and unblock the *SIGVTALRM* signal. We set up sigaction with the sigvtalrm_handler function and set the
flags to 0 as default (same as using signal). We also set up a timer that would stop after 10 milliseconds using the *timer.it_value.tv_usec* variable and repeat every 10 milliseconds after that using the *timer.it_interval.tv_usec* variable.

## Testing Preempt
We test our queue using test_preempt.c. Inside this file, we created 3 threads with no yield functions. The output is unpredictable, but all the threads output its respective print statement which implies that preempt is working. NOTE: In order to test preempt you will need to set *allow_preempt* static variable to 1 at the top of *uthread.c* file.

## Sources we used to write preempt
We used the example from this [website](http://www.informit.com/articles/article.aspx?p=23618&seqNum=14) to help us write *preempt_start()*.