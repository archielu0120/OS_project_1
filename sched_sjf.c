// In SJF root is the first process in waiting list

#define _GNU_SOURCE
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include "util.h"

char proc_name[MAX_PROC_NUM][MAX_NAME_LEN];
unsigned int ready_time[MAX_PROC_NUM], burst_time[MAX_PROC_NUM];

extern int p_num;
extern int process_table[MAX_PROC_NUM];
extern int ready_num;
extern int run_num;
extern int Used;
extern pid_t proc_id[MAX_PROC_NUM];
extern int flag;

Node  *root = NULL;
void next_pocess_preparation() {
	int selected = root -> index;
	SET_PRIORITY(proc_id[selected], SCHED_FIFO, MID_PRIOR);
}

void decrease_current_next() {
	if(!Used || root == NULL){
		return;
	}
	int selected = root ->  index;
	SET_PRIORITY(proc_id[selected], SCHED_FIFO,  LOW_PRIOR);
}

void run_next() {
	int selected = root ->  index;
	root =  pop_minheap(root);
	Used = 1;
	SET_PRIORITY(proc_id[selected], SCHED_FIFO, HIGH_PRIOR);
}

void  check_and_assign_schedule() {
	if(Used && root != NULL)
		next_pocess_preparation();
	
	if(!Used && root != NULL)
		run_next();
	
}

//signal handle
void child_end(int signum) { 
    static int finished = 0;
    wait(NULL);
    run_num++;
    finished++;
    Used = 0;
    if(finished == p_num){
        exit(0);
    }
}

int main (void)
{    
    READ_TASK_INFO();
    
    for (int i = 0; i < p_num; i++){
        process_table[i] = i;
    }   

    index_sort(p_num, process_table, burst_time);	//sort by burst first
    index_sort(p_num, process_table, ready_time);	//sort by ready time

    WAIT_ALL_CHILD();
    
    int next_ready = process_table[ready_num];  
    
    flag = 0;
    for (int time = 0, i = p_num; i > 0; ++time) {

        check_and_assign_schedule();
        while (ready_num < p_num && time == ready_time[next_ready]) {
            decrease_current_next();	//a new ready process's priority might higher than currnet root -> decrease current root
            fork_child(&proc_id[next_ready], proc_name[next_ready], next_ready, burst_time[next_ready]);
            root =  add_to_minheap(root, burst_time[next_ready], next_ready);
            next_ready = process_table[++ready_num];
	    check_and_assign_schedule();
        }
	    
        unit_time();
    }
}
