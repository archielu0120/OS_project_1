// In PSJF the root is the running process if someone is running

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
extern int Used;
extern pid_t proc_id[MAX_PROC_NUM];

Node *root = NULL;
int finished = 0;
int last_executed_time = 0;

#define BLK_SIGNALS() 	\
	sigset_t myset, oldset;		\
	do{		\
		sigfillset(&myset);			\
		sigprocmask(SIG_BLOCK,&myset,&oldset);	\
	}while(0)

#define UNBLK_SIGNALS() sigprocmask(SIG_SETMASK, &oldset, NULL)

#define DETECT_SIGNAL() do{			\
		BLK_SIGNALS();	\
		while(root != NULL && waitpid(proc_id[root -> index], NULL, WNOHANG) != 0){	\
			finished++;		\
   			root = pop_minheap(root);	\
    		Used = 0;	\
    		if(finished == p_num) exit(0);	\
		}		\
		UNBLK_SIGNALS();	\
	}while(0)


void child_end(int signum) { 
    wait(NULL);
    finished++;
    root = pop_minheap(root);
    Used = 0;
    if(finished == p_num){
        exit(0);
    }
}

void decrease_first_two_pririty() {
	BLK_SIGNALS();
	if(!Used || root == NULL)
		return;

	int running = root -> index;
	burst_time[running] -= last_executed_time;
	last_executed_time =  0;
	root = pop_minheap(root);

	DETECT_SIGNAL();
	if(root){
		int waiting = root -> index;
		SET_PRIORITY(proc_id[waiting], SCHED_FIFO, LOW_PRIOR);
	}
	SET_PRIORITY(proc_id[running], SCHED_FIFO, LOW_PRIOR);
	root = add_to_minheap(root, burst_time[running], running);
	UNBLK_SIGNALS();
}

void next_proceess_preparation() {
	BLK_SIGNALS();
	int running = root -> index;
	root = pop_minheap(root);
	SET_PRIORITY(proc_id[running], SCHED_FIFO,  HIGH_PRIOR);
	DETECT_SIGNAL();
	if(root){
		int waiting = root -> index;
		SET_PRIORITY(proc_id[waiting],  SCHED_FIFO, MID_PRIOR);
	}
	root = add_to_minheap(root, burst_time[running], running);
	UNBLK_SIGNALS();
}

void run_next() {
	BLK_SIGNALS();
	int selected = root -> index;
	SET_PRIORITY(proc_id[selected], SCHED_FIFO, HIGH_PRIOR);
	last_executed_time = 0;
	Used = 1;
	UNBLK_SIGNALS();
}



void check_and_assign_schedule() {
	DETECT_SIGNAL();
	BLK_SIGNALS();
	if(Used && root)
		next_proceess_preparation();
	
	if(!Used && root)
		run_next();

	UNBLK_SIGNALS();
}



int main(void){
	READ_TASK_INFO();
	for(int i = 0; i<p_num; i++){
		process_table[i] = i;
	}

	index_sort(p_num,process_table,burst_time);
	index_sort(p_num,process_table,ready_time);
	WAIT_ALL_CHILD();
	int next_ready = process_table[ready_num];
	for(int time = 0 ; ; ++time){
		check_and_assign_schedule();
		while(ready_num < p_num &&  ready_time[next_ready] == time){
			decrease_first_two_pririty();
			fork_child(&proc_id[next_ready],proc_name[next_ready], next_ready, burst_time[next_ready]);
			root = add_to_minheap(root, burst_time[next_ready], next_ready);
			next_ready = process_table[++ready_num];
			check_and_assign_schedule();
		}
		unit_time();
		last_executed_time++;
		DETECT_SIGNAL();
	} 

	
}
