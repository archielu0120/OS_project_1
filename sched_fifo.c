#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include "util.h"

char proc_name[MAX_PROC_NUM][MAX_NAME_LEN];
unsigned int ready_time[MAX_PROC_NUM], burst_time[MAX_PROC_NUM];

extern int p_num;
extern int process_table[MAX_PROC_NUM];
extern int ready_num;
extern int run_num;
extern int Used;
extern pid_t proc_id[MAX_PROC_NUM];

void child_end(int signum) {
    static int finished = 0;
    wait(NULL);
    run_num++;
    finished++;
    Used = 0;
    if(finished == p_num)exit(0);
}

void next_process_preparation() {
    int selected_proc = process_table[run_num + 1];
    SET_PRIORITY(proc_id[selected_proc], SCHED_FIFO, MID_PRIOR);
}

void run_next() {
    int selected_proc = process_table[run_num];			
    SET_PRIORITY(proc_id[selected_proc], SCHED_FIFO, HIGH_PRIOR);	
    Used = 1;						
}

void check_and_assign_priority() {
    if (!Used && run_num < ready_num)		//cpu is not occupied and has waiting process
        run_next();
    if (Used && run_num + 1 < ready_num)	   
        next_process_preparation();
}

int main (void)
{    
    READ_TASK_INFO();
    
    for (int i = 0; i < p_num; i++){
        process_table[i] = i;
    }   

    index_sort(p_num, process_table, ready_time);

    WAIT_ALL_CHILD();
    
    int next_ready = process_table[ready_num];    

    for (int time = 0, i = p_num; i > 0; ++time) {
        check_and_assign_priority();
        while (ready_num < p_num && time == ready_time[next_ready]) {
            fork_child(&proc_id[next_ready], proc_name[next_ready], next_ready, burst_time[next_ready]);
            next_ready = process_table[++ready_num];
            check_and_assign_priority();
        }
        unit_time();
    }
}