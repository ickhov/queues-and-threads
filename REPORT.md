# ECS150P2

**Uthread API**

In our _uthread_create()_ function, before creating a new thread with a new TCB
struct, we check to see if it is the first thread being created. If it is the
first thread created, we will first create a TCB struct for the already running
thread in main and set it's stack to NULL and it's TID to 0.

_TCB Struct_

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

queue of threads The threads will be handled with a FIFO queue. If a thread
becomes blocked from joining a child, it will be added to a queue of blocked
threads.

queue of blocked threads

used in uthread_join and uthread_exit **searching the queue** find_tcb_by_state

find_tcb_by_tid

**preempt API**

```c
void sigvtalrm_handler(int signum)
```
The alarm handler function will call uthread_yield every time that there is a
timer interrupt. 

```c
void preempt_disable(void)
```



```c
void preempt_enable(void)
```



```c
void preempt_start(void)
```



**Queue API**

*queue_t queue_create(void)*
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
will assign the oldest item to \*data and then remove it from the queue. After
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