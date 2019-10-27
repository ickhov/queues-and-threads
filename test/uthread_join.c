/*
 * Testing join by joining thread 1 with main thread and
 * thread 2 with thread 1 so the output should be:
 *
 * thread3: self is 3
 * thread2: self is 2 and retval is 0
 * thread1: self is 1 and retval is 0
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void* arg)
{
	uthread_yield();
	printf("thread3: self is %d\n", uthread_self());
	return 0;
}

int thread2(void* arg)
{
	int tid = uthread_create(thread3, NULL);
	int retval;
	uthread_join(tid, &retval);
	printf("thread2: self is %d and retval is %d\n", uthread_self(), retval);
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
