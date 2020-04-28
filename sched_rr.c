#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include "util.h"

#define TIME_SLICE 500
#define MAX_QUEUE_SIZE 1000007


char proc_name[MAX_PROC_NUM][MAX_NAME_LEN];
unsigned int ready_time[MAX_PROC_NUM], burst_time[MAX_PROC_NUM];

extern int p_num;
extern int process_table[MAX_PROC_NUM];
extern int ready_num;
extern int Used;
extern pid_t proc_id[MAX_PROC_NUM];

int q[MAX_QUEUE_SIZE];
int clock,blk,head,tail;



void push(int idx) {
	q[tail++] = idx;
}

void pop() {
	if(head <  tail)
		head++;	
}

int get_front() {
	return q[head];
}

void child_end(int signum) {
    static int finished = 0;
    wait(NULL);
    clock = 0;
    blk = 0;
    finished++;
    Used = 0;
    pop();
    if(finished == p_num)
        exit(0);
}

void run_next() {
	clock = 0;
	int selected_proc = q[head];
	if(burst_time[selected_proc]  <= TIME_SLICE)	//detect resume burst time
		blk = 1;
	SET_PRIORITY(proc_id[selected_proc], SCHED_FIFO, HIGH_PRIOR);
	Used = 1;
}

void next_process_preparation() {
    int selected_proc = q[head+1];
    SET_PRIORITY(proc_id[selected_proc], SCHED_FIFO, MID_PRIOR);
}

void check_and_assign_priority() {
    if (!Used && tail - head >= 1)//cpu is not occupied and has waiting process
    	run_next();
    		
    if (Used && tail - head  >=  2)	   
        next_process_preparation();
}




int main(void){
	READ_TASK_INFO();
	for(int i = 0; i<p_num; i++){
		process_table[i] = i;
	}
	index_sort(p_num,process_table,ready_time);
	WAIT_ALL_CHILD();

	int next_ready = process_table[ready_num]; 
	for (int time = 0, i = p_num; i > 0; ++time) {
        check_and_assign_priority();
        while (ready_num < p_num && time == ready_time[next_ready]) {
            fork_child(&proc_id[next_ready], proc_name[next_ready], next_ready, burst_time[next_ready]);
            push(next_ready);
            next_ready = process_table[++ready_num];
            check_and_assign_priority();
        }
        unit_time();
        clock++;
   		if(Used && blk != 1 && clock == TIME_SLICE){
   			int idx = get_front();
   			pop();
   			burst_time[idx] -= TIME_SLICE;
   			SET_PRIORITY(proc_id[idx],SCHED_FIFO,LOW_PRIOR);
   			push(idx);
   			Used = 0;
   			check_and_assign_priority();
   		}
    }


    

}