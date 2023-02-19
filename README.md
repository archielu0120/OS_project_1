# report

# 環境

|       Platform        | Programming Language | OS    |     Kernel     |
|:---------------------:|:--------------------:| ----- |:--------------:|
| VirtualBox 6.0 on Mac |          C          | Linux | Ubuntu 4.14.25 |

# 設計
## 架構
使用兩顆CPU 來達成排程效果。 CPU 0 負責 schedule  ， CPU 1 負責跑task 。 在讀進資料後， 先作排序 ， 並在  CPU 0 跑迴圈作計時。 當達到執行條件時 ， fork 出一個 child process 並使用 sched_setaffinity() 將其送到   CPU  1 。 每個process 能擁有3種 priiority  。 在一開始被 fork 的時候是 MID_PRIOR 且馬上被設為 LOW_PRIOR 。 在 waiting list 前端準備要被執行的時候設為 MID_PRIOR 。 執行中的 process 為 HIGH_PRIOR 。 而當某個 child process 結束可以讓出 CPU 的時候 ， 利用 SIGCHLD 通知 parent process 自己已經結束了。

### FIFO
實作下最直觀的FIFO ， ㄧ開始將input 照ready time 排序。
``` c=
check_and_assign_priority() {
    if(CPU 1 is occupied  &&  there exist waiting process) {
        set first process in waiting list to MID_PRIOR
    }
    if(CPU 1 is not occupied && there exist waiting process) {
        set first process in waiting list to HIGH_PRIOR (i.e. run that process)
    }
}
```
```
sched_fifo {

    Sort()     //By ready time
    
    WAIT ALL CHILD
    
    while(! all task has finished){    // 

        check_and_assign_priority();    //check CPU 1 's state and a    ssign new priority for waiting queue
    
        wihle(exist new ready process){
            fork_child();
            check_and_assign_priority();
        }
    
        unit_time();
    }
 }
```

### RR
用一個 first in first out queue q 把 waiting process 放到裡面。  在parent process 設一個 clock ， 當clock ==  TIME_SLICE 的時候 ，把running process  set  LOW_PRIOR 。 並將他一到 q 的尾巴。 head 為目前正在 run 的 process。

```
check_and_assign_priority() {
    if(CPU 1 is occupied  &&  there exist waiting process) {
        set first process in waiting list to MID_PRIOR
    }
    if(CPU 1 is not occupied && there exist waiting process) {
        set first process in waiting list to HIGH_PRIOR (i.e. run that process)
    }
```
```
sched_rr {

    Sort();    //By ready time
    
    WAIT ALL CHILD
    
    whilie(! all task has finished) {
        check_and_assiign_priority();
        while(exist ready process) {
            fork_child();
            push it into q
            check_and_assign_priority();
        }
        clock++;
        uniit_time();
        if(clock == TIME_SLICE && CPU 1 is occupied) {
            burst_time of running process -= TIME_SLICE;
            pop running process from q
            set running process LOW_PRIOR
            push it back to q
            Ocuupied = false
            check_and_assign_priority();
        }
    }
}
```
### SJF
SJF 中 在某個時間點有一個新的process ready 了 並且 CPU 1 剛好被釋放。 這時 waiting list 的 first process 不一定是最優先的（因為最新ready process 的 burst time 可能更小 )  。 因此， 先將waiting list 中 first process set LOW_PRRIOR （decrease_next() ) ， 並將最新 ready process 插入 min heap 中做更新 。 如此操作完之後才能保證是 SJF 。 heap -> root 為 waiting list 中最優先的 process。

```
check_and_assign_priority() {
    if(CPU 1 is occupied  &&  there exist waiting process) {
        set first process in waiting list to MID_PRIOR
    }
    if(CPU 1 is not occupied && there exist waiting process) {
        set first process in waiting list to HIGH_PRIOR (i.e. run that process)
    }
```
```
sched_sjf {

    Sort();      //By burst time  
    Sort();     //By ready time
    
    WAIT ALL CHILD
    
    while(! all task has finished){    
  
        wihle(exist new ready process){
            decrese_next();
            fork_child();
            insert new ready process into min heap
            flag = 1;
        }
        
        if(flag){
            check_and_assign_priority();
        }
        flag = 0;
        unit_time();
    }
```
### PSJF
PSJF 中 min heap 裡的root 不是first waiting 而是running process 。  因為 preemtive ， 所以只要有新的ready process ， running process 與 first waiting 就要更新一次，並且在丟入min heap 比較。 另外 ， 為了避免 race condition 可以用 sigprocmask 去 block SIGCHLD 。
```
decrease_priority() {
    if(CPU 1 is not occupied && root)
    running = root;
    burst time of running -= counting
    counting = 0;
    pop root from min heap;
    if (root) 
        set root to LOW_PRIOR
    set running to LOW_PRIOR
    insert running back to min heap
    
}

```

```
check_and_assign_priority() {
    if(CPU 1 is occupied  &&  there exist waiting process) {
        set first process in waiting list to MID_PRIOR
    }
    if(CPU 1 is not occupied && there exist waiting process) {
        set first process in waiting list to HIGH_PRIOR (i.e. run that process)
    }
```
```
sched_psjf {

    Sort();      //By burst time  
    Sort();     //By ready time
    
    WAIT ALL CHILD
    
    while(! all task has finished){    

        check_and_assign_priority();    
    
        wihle(exist new ready process){
            decrese_priority();
            fork_child();
            insert new ready process into min heap
            check_and_assign_priority();
        }
        counting++;
        unit_time();
    }
```






# 可能造成誤差的原因：
1. 設計上為schedule 和 task 在不同 CPU 上跑。 就算在同一顆 CPU 上跑每次unit_time() 都不同，在不同 CPU 上誤差可能會越大。
2. 照理來說，實作下CPU排程還需要處理資料結構等程式本身問題，可能會造成超時的誤差。然而，數據中發現有負的誤差，這表示系統本身也可能會有context switch 之類等等自身運作上的成本。 因此， 實驗當時的 CPU 負載狀況也是影響結果重要的因素之一。
