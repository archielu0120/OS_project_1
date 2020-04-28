#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include <assert.h>
#include<signal.h>
#include<sched.h>
#include<unistd.h>
#define MAX_PROC_NUM 10000
#define MAX_NAME_LEN 32
#define MAX_PROC_NAME 128
#define LOW_PRIOR (int)30
#define MID_PRIOR (int)50
#define HIGH_PRIOR (int)80
#define TIME_UNIT 1000000UL



#define READ_TASK_INFO() do {					\
	assert(scanf("%d", &p_num) == 1);			\
	for (int i = 0; i < p_num; ++i) {			\
	    assert(scanf("%s%u%u", proc_name[i], &ready_time[i], &burst_time[i]) == 3);	\
	}							\
    } while(0)

#define WAIT_ALL_CHILD() do {			\
	struct sigaction act;			\
	act.sa_flags = 0;			\
	act.sa_handler = child_end;		\
	sigfillset(&act.sa_mask);		\
	sigaction(SIGCHLD, &act, NULL);		\
    } while(0)


// Set process pid to CPU x
#define SELECT_CPU(pid, x) do {						\
	cpu_set_t mask;							\
	CPU_ZERO(&mask);						\
	CPU_SET((x), &mask);						\
	if (sched_setaffinity((pid), sizeof(mask), &mask) != 0) {	\
	    printf("Set Affinity Fail\n");				\
	}								\
    } while (0)

#define SET_PRIORITY(pid, policy, prior) do {			\
	struct sched_param param;					\
	param.sched_priority = prior;				\
	if (sched_setscheduler(pid, policy, &param) != 0) {		\
	    printf("Set Priorrity Fail\n");				\
	}								\
    } while (0)

typedef struct node{
	int value, size, index, depth;
	struct node *left;
	struct node *right;
}Node;



Node *create_node(int vlaue, int index);
int min_heap_size(Node *a);
Node *merge(Node *a, Node *b);
Node *add_to_minheap(Node *r, int value, int index);
Node *pop_minheap(Node *r);
void fork_child(pid_t *pid, char *name, int index, int B);
void inddex_sort(int n, int index[], unsigned int R[]);
void _sort(int l, int r, int index[], unsigned int R[]);
Node *create_node(int vlaue, int index);
void unit_time();