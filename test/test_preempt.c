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

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void* arg)
{
	printf("thread3: self is %d\n", uthread_self());
	return 0;
}

int thread2(void* arg)
{
	uthread_create(thread3, NULL);
	printf("thread2: self is %d\n", uthread_self());
	return 0;
}

int thread1(void* arg)
{
	int tid = uthread_create(thread2, NULL);
	int retval;
	uthread_join(tid, &retval);
	printf("thread1: self is %d and retval is %d\n", uthread_self(), retval);
	uthread_yield();
	return 0;
}

int main(void)
{
	uthread_join(uthread_create(thread1, NULL), NULL);
	return 0;
}
