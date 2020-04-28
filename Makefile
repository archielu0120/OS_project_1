all: process scheduler
process:
        gcc -O2 process.c func.c -o process
scheduler:
        gcc -O2 sched_fifo.c func.c index_sort.c -o sched_fifo
        gcc -O2 sched_rr.c func.c index_sort.c -o sched_rr
        gcc -O2 sched_sjf.c func.c index_sort.c min_heap.c -o sched_sjf
        gcc -O2 sched_psjf.c func.c index_sort.c min_heap.c -o sched_psjf
        gcc -O2 main.c func.c index_sort.c -o main
clean:
        rm -rf process sched_fifo sched_rr sched_sjf main
