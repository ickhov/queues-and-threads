#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

static sigset_t sig_mask;

void sigvtalrm_handler(int signum) {
    printf("Timer interrupt\n");
    
    uthread_yield();
}

void preempt_disable(void)
{
    // block SIGVTALRM
    if (sigprocmask(SIG_BLOCK, &sig_mask, NULL) < 0) {
        printf("Failed to set signal mask!\n");
        exit(1);
    }

    printf("Block SIGVTALRM\n");
}

void preempt_enable(void)
{
    // unblock SIGVTALRM
    if (sigprocmask(SIG_UNBLOCK, &sig_mask, NULL) == -1) {
        printf("Failed to set signal mask!\n");
        exit(1);
    }

    printf("Unblock SIGVTALRM\n");
}

void preempt_start(void)
{
    // sig_mask is static
    // use to block/unblock signal
    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIGVTALRM);

    // set up sigaction by setting handler and appropriate flag
    // flag 0 is defualt for signal so we use the same here
    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    sig_act.sa_handler = &sigvtalrm_handler;
    sigaction(SIGVTALRM, &sig_act, NULL);

    // set a timer to interrupt in 10 milliseconds
    // and repeat every 10 milliseconds
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = HZ * 100;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = HZ * 100;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}