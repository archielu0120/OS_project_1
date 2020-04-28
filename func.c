#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sched.h>
#include "util.h"

int p_num;
int process_table[MAX_PROCESS_NUM];
int ready_num = 0;
int run_num = 0;
int Used = 0;
pid_t proc_id[MAX_PROCESS_NUM];

void unit_time() {
    volatile unsigned long i;
    for (i = 0; i < TIME_UNIT; i++);
}

void fork_child(pid_t *pid, char *name, int I, int B)
{
    if ((*pid = fork()) < 0){
        printf("fork error\n");
    }
    if (*pid == 0) {
        char burst_time[32], index[16];
        sprintf(index, "%u", I);
        sprintf(burst_time, "%u", B);
        execl("./process", "./process", name, index, burst_time, NULL);
    }
    if (*pid > 0) {
        fflush(stderr);
        SET_PRIORITY(*pid, SCHED_FIFO, LOW_PRIOR);
        SELECT_CPU(*pid, 1);
    }
}