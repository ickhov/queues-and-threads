#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <stdio.h>

#include "queue.h"

struct queue {
    void **item;
    int head;
    int tail;
    int size;
};

queue_t queue_create(void)
{
    struct queue* q = (struct queue*) malloc(sizeof(struct queue));

    // if NULL, then no more memory space so return NULL
    if (q == NULL) {
        return NULL;
    }

    q->item = NULL;
    q->head = 0;
    q->tail = 0;
    q->size = 0;

    return q;
}

int queue_destroy(queue_t queue)
{
    // if queue is NULL or if queue is not empty
    if (queue == NULL || (queue->head != queue->tail)) {
        return -1;
    }

    for (int i = queue->head; i < queue->tail; i++) {
        if (queue->item[i] != NULL) {
            free(queue->item[i]);
        } 
    }

    free(queue->item);
    free(queue);

    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    // check if queue or data is NULL
    if (queue == NULL || data == NULL) {
        return - 1;
    }

    if (queue->size == queue->tail) {
        // increment 5 spaces each time we need to realloc
        queue->size = queue->size + 5;
        // allocate more space for the data in queue
        queue->item = (void **) realloc(queue->item, queue->size * sizeof(void*));

        // check for memory allocation error
        if (queue->item == NULL) {
            return -1;
        }
    }

    // add the data to queue
    queue->item[queue->tail] = data;

    // increment the tail
    queue->tail = queue->tail + 1;

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    // check if queue is null
    // if head and tail are the same, then queue is empty
    if (queue == NULL || (queue->head == queue->tail)) {
        return - 1;
    }

    // assign oldest item to data
    *data = queue->item[queue->head];

    // remove it from queue
    queue->item[queue->head] = NULL;

    // update head to the next oldest item
    queue->head = queue->head + 1;

    // check if data is null
    if (*data == NULL) {
        return -1;
    }

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    // check if queue or data is NULL
    if (queue == NULL || data == NULL) {
        return - 1;
    }

    for (int i = 0; i < queue->tail; i++) {
        // check if the pointers are the same
        if (queue->item[i] == data) {

            // shift every element back once
            for (int j = i; j < queue->tail - 1; j++) {
                queue->item[i] = queue->item[i + 1];
            }

            queue->item[queue->tail - 1] = NULL;

            // decrement tail to account for the removal
            queue->tail = queue->tail - 1;

            return 0;
        }
    }

    // data was not found in the queue
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    // check if queue or func is NULL
    if (queue == NULL || func == NULL) {
        return - 1;
    }

    // iterate through the item
    for(int i = queue->head; i < queue->tail; i++) {

        // check if func returns 1
        if (func(queue->item[i], arg)) {

            //if data pointer is not NULL, then set data equal to current item
            if (data != NULL)
            {
                *data = queue->item[i];
            }
            break;
        }
    }

    return 0;
}

int queue_length(queue_t queue)
{
    return queue->tail - queue->head;
}