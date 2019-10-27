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
    int data = 3;
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
    assert(ptr == &data);

	queue_dequeue(q, (void**)&ptr);

	// -1 if queue is empty
	retval = queue_dequeue(q, (void**)&ptr);
	assert(retval == -1);
}

void test_length()
{
	queue_t q = NULL;
    int data = 3, data2 = 4, data3 = 5;
	
    q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);

	int retval = queue_length(q);
	assert(retval == 3);
}

int main(void)
{
	test_create();
	test_destroy();
	test_enqueue();
	test_dequeue();
	test_length();
	return 0;
}
