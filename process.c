#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
#include "util.h"

int main(int argc, char **argv) {
    
    if (argc != 4) {
        fprintf(stderr, "Exec Process Fail\n");
        return 0;
    }
    
    char *name = argv[1];
    int idx = atoi(argv[2]);
    int burst_time = atoi(argv[3]);
    pid_t pid = getpid();
    struct timespec start;
    struct timespec end;
    printf("%s %d\n", name, pid); 
    getnstimeofday(&start);
    for (int i = 0; i < burst_time; i++)
        unit_time();
    getnstimeofday(&end);
   
    syscall(333, "[Project1]", pid, &start, &end);

    return 0;
}