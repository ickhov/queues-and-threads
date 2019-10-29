/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

void test_create()
{
    queue_t q;

    q = queue_create();
    assert(q != NULL);

    queue_destroy(q);
}

void test_destroy()
{
    queue_t q = NULL;
	int data = 3, *ptr;
	int retval = 0;

	// -1 when trying to destroy NULL queue
	retval = queue_destroy(q);
	assert(retval == -1);

    q = queue_create();
	queue_enqueue(q, &data);

	// -1 when trying to destroy queue that's not empty
	retval = queue_destroy(q);
	assert(retval == -1);

	queue_dequeue(q, (void**)&ptr);

	// 0 when trying to destroy empty queue
	retval = queue_destroy(q);
	assert(retval == 0);
}

void test_enqueue()
{
    queue_t q;
    int data = 3, *ptr;
	int retval = 0;

	// -1 when queue is NULL
	retval = queue_enqueue(NULL, NULL);
	assert(retval == -1);
	
    q = queue_create();

	// -1 when data is NULL
	retval = queue_enqueue(q, NULL);
	assert(retval == -1);

	// 0 if data was successfully enqueued in queue.
    retval = queue_enqueue(q, &data);
    assert(retval == 0);

    queue_dequeue(q, (void**)&ptr);
    queue_destroy(q);
}

void test_dequeue()
{
    queue_t q = NULL;
    int data = 3, data2 = 4, *ptr = NULL;
	int retval = 0;

	// -1 when queue is NULL
	retval = queue_dequeue(NULL, NULL);
	assert(retval == -1);
	
    q = queue_create();

	queue_enqueue(q, &data);
	queue_enqueue(q, &data2);

	// 0 if oldest data was successfully dequeued from queue.
    queue_dequeue(q, (void**)&ptr);
	assert(*ptr == data);
    assert(ptr == &data);

	queue_dequeue(q, (void**)&ptr);

	// -1 if queue is empty
	retval = queue_dequeue(q, (void**)&ptr);
	assert(retval == -1);

    queue_destroy(q);
}

void test_delete() {
    queue_t q;
    int data = 3, data2 = 4, data3 = 5, *ptr;
    int retval = 0;

	// -1 when the queue is null
    retval = queue_delete(NULL, (void**)&data);
    assert(retval ==-1);

    q = queue_create();

	// -1 when the data is null
    retval = queue_delete(q, NULL);
    assert(retval ==-1);

    // -1 when the data is not in the queue
    retval = queue_delete(q, (void**)&data);
    assert(retval ==-1);

    queue_enqueue(q, &data);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);

    // 0 when data is successful
    retval = queue_delete(q, (void**)&data);
    assert(retval == 0);

    // data2 should be dequeued since we 
	// already deleted the first data
	// MAKE SURE DELETE REALLY WORKED
    queue_dequeue(q, (void**)&ptr);
	assert(*ptr == data2);
    assert(ptr == &data2);

    queue_dequeue(q, (void**)&ptr);
    queue_destroy(q);
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;

    *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;

    if (*a == match)
        return 1;

    return 0;
}

void test_iterator()
{
    queue_t q = NULL;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *ptr;

	int retval = 0;

    // -1 when queue is NULL
    retval = queue_iterate(q, inc_item, (void*)3, (void**)&ptr);
    assert(retval == -1);

    /* Initialize the queue and enqueue items */
    q = queue_create();

	// -1 when function is NULL
    retval = queue_iterate(q, NULL, (void*)3, (void**)&ptr);
    assert(retval == -1);

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Add value '1' to every item of the queue */
    queue_iterate(q, inc_item, (void*)1, NULL);
    assert(data[0] == 2);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    assert(ptr != NULL);
    assert(*ptr == 5);
    assert(ptr == &data[3]);

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_dequeue(q, (void**)&ptr);

    queue_destroy(q);
}

void test_length()
{
	queue_t q = NULL;
    int data = 3, data2 = 4, data3 = 5, *ptr;
	int retval = 0;
	
    q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);

	retval = queue_length(q);
	assert(retval == 3);

	queue_dequeue(q, (void**)&ptr);

	retval = queue_length(q);
	assert(retval == 2);

    queue_dequeue(q, (void**)&ptr);

	retval = queue_length(q);
	assert(retval == 1);

    queue_dequeue(q, (void**)&ptr);
    queue_destroy(q);
}

int main(void)
{
	test_create();
	test_destroy();
	test_enqueue();
	test_dequeue();
	test_delete();
	test_iterator();
	test_length();
	return 0;
}
