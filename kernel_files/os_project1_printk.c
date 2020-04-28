#include<linux/linkage.h>
#include<linux/kernel.h>
#include<linux/ktime.h>
#include<linux/timekeeping.h>
#include<linux/types.h>

asmlinkage void sys_os_project1_printk(char *tag, pid_t pid, struct timespec *start, struct timespec *end)
{
        printk("%s %d %lu.%09lu %lu.%09lu\n",tag, pid, start->tv_sec, start->tv_nsec, end->tv_sec, end->tv_nsec);
        return;
}

