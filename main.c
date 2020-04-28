#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sched.h>
#include "util.h"

int main() {
    
    char policy[8];
    assert(scanf("%s", policy) == 1);
    SELECT_CPU(getpid(), 0);
    SET_PRIORITY(getpid(), SCHED_FIFO, MID_PRIOR);

    fflush(stdin);
    if(strncasecmp(policy,"FIFO",sizeof(policy))  ==  0){
        execl("./sched_fifo", "./sched_fifo", NULL);
    }
    else if(strncasecmp(policy,"RR",sizeof(policy))  ==  0){
        execl("./sched_rr", "./sched_rr", NULL);
    }
    else if(strncasecmp(policy,"SJF",sizeof(policy))  ==  0){
        execl("./sched_sjf", "./sched_sjf", NULL);
    }
    else if(strncasecmp(policy,"PSJF",sizeof(policy))  ==  0){
        execl("./sched_psjf", "./sched_psjf", NULL);
    }
    else{
        printf("Not match\n");
    }
    
    return 0;
}