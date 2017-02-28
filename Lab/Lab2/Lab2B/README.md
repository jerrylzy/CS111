# Lab 2B: Lock Granularity and Performance
## File description
* `SortedList.*`: a C module that implements insert, delete, lookup, and length methods for a sorted doubly linked list.
* `lab2_list.c`: a C program that drives one or more parallel threads that do operations on a shared linked list, and reports on the final list and performance.
* `Makefile`: a Makefile that supports `build, tests, graphs, tarball, clean` options.
* `lab_2b_list.csv`: results for all performance tests.
* `lab2_*.csv`: results from lab2a; I don't think `lab2_add.c` is needed, but there is no follow up on piazza.
* `lab2b_1.png`: throughput vs number of threads for mutex and spin-lock synchronized adds and list operations.
* `lab2b_2.png`: mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
* `lab2b_3.png`: number of successful iterations for each synchronization method.
* `lab2b_4.png`: throughput vs number of threads for mutexes with partitioned lists.
* `lab2b_5.png`: throughput vs number of threads for spin-locks with partitioned lists.
* `lab2.gp`: GNUPlot script to plot necessary graphs.
* `profile.gperf`: Gperf tool output.
* `README.txt`: A markdown readme file forced to have `.txt` extension.

## `make check` Methology
This option invokes the file `test.sh` to perform specified test cases.

## Note 
### About GNUPlot
`Makefile` assumes that the machine has already installed GNUPlot 5.0 or newer. SEASNet server `lnxsrv09` installed it under `/usr/local/cs/bin`.
### About Gperftool
`Makefile` assumes that the machine has already installed GPerftool 2.5 in `$HOME/gperftools`. 

## Questions
### 2.3.1 Cycles in the basic implementation:
1. Where do you believe most of the cycles are spent in the 1 and 2-thread tests (for both add and list)?  Why do you believe these to be the most expensive parts of the code?

        For add, locks probably have the same cost as addition since there isn't much contention for locks. 
        For list, most time are spent in the list operations because it is much more expensive than simple checks for locks.
        The above are my speculations since locks are just checks 
        and we have enough CPU to accomodate 1 and 2 threads, which means we don't have much context-switch overhead.

2. Where do you believe most of the time/cycles are being spent in the high-thread spin-lock tests?

        For add, there is no question most time are spent in the spin-lock checks and spins.
        For list, if there are really a lot of threads, spin-lock may cost much more in CPU time.

3. Where do you believe most of the time/cycles are being spent in the high-thread mutex tests?
    
        For add, most CPU cycles are spent on context-switches.
        For list, if the list length is really long, then most CPU cycles will be spent on list operations.
        If the list length is not very long, then most CPU cycles will be spent on context-switches 
        since context-switch are very expensive.

### 2.3.2 Execution Profiling:
1. Where (what lines of code) are consuming most of the cycles when the spin-lock version of the list exerciser is run with a large number of threads?

        The line for spin-lock check consumes most of the cycles according to output from gperftool.

2. Why does this operation become so expensive with large numbers of threads?

        This happens because lock contention happens far often for a large number of threads 
        and each spin-lock will do nothing but check if it can get the lock, which wastes a lot of CPU cycles.
        If we run 32 threads at a time, we will see that > 99% of CPU cycles are spent on spin lock checks.

### 2.3.3 Mutex Wait Time:
Look at the average time per operation (vs # threads) and the average wait-for-mutex time (vs #threads).  
1. Why does the average lock-wait time rise so dramatically with the number of contending threads?

        Because more threads are competing for a single lock, the time for a thread to grab a lock increases.

2. Why does the completion time per operation rise (less dramatically) with the number of contending threads?

        Contention rises as the number of thread increases and 
        completion time counts the time for contention / context-switch. 
        This is why completion time per operation increases a little.

3. How is it possible for the wait time per operation to go up faster (or higher) than the completion time per operation?

        The completion time counts the wall time for the whole operation. 
        The wait time, however, contains the wall time for each individual thread. 
        Threads have many overlapping perods of wait time, so the wait time per operation increases much faster.

### 2.3.4 Performance of Partitioned Lists
1. Explain the change in performance of the synchronized methods as a function of the number of lists.

        More sublists means, for each list, the length is smaller, and the possibility of contention of a lock
        (for a single list) is decreased.

2. Should the throughput continue increasing as the number of lists is further increased?  If not, explain why not.

        The throughput depends on the CPU, the maximum number of threads that can run simutaneously on the CPU. 
        If you have 8 threads, 16 sublists, the possibility of competing for a lock will be small. 
        There will be a point where the possibility of contention is almost 0.

3. It seems reasonable to suggest the throughput of an N-way partitioned list should be equivalent to the throughput of a single list with fewer (1/N) threads.  Does this appear to be true in the above curves?  If not, explain why not.


        No. Partitioning a list also shortens the list and reduces the number of time in the critical section 
        due to a smaller length. Thus the possibility of a contention is decreased when we partition a list.

