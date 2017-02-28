# Lab 2A: Races and Synchronization
## File description
* `lab2_add.c`: a C program that implements and tests a shared variable add function.
* `SortedList.*`: a C module that implements insert, delete, lookup, and length methods for a sorted doubly linked list.
* `lab2_list.c`: a C program that tests a shared sorted list.
* `Makefile`: a Makefile that supports `build, tests, graphs, tarball, clean` options.
* `lab2_add.csv`: results for all of the Part-1 tests.
* `lab2_list.csv`: results for all of the Part-2 tests.
* `lab2_add-1.png`: threads and iterations required to generate a failure (with and without yields).
* `lab2_add-2.png`: average time per operation with and without yields.
* `lab2_add-3.png`: average time per (single threaded) operation vs. the number of iterations.
* `lab2_add-4.png`: threads and iterations that can run successfully with yields under each of the three synchronization methods.
* `lab2_add-5.png`: average time per (multi-threaded) operation vs. the number of threads, for all four versions of the add function.
* `lab2_list-1.png`: average time per (single threaded) unprotected operation vs. number of iterations (illustrating the correction of the per-operation cost for the list length).
* `lab2_list-2.png`: threads and iterations required to generate a failure (with and without yields).
* `lab2_list-3.png`: iterations that can run (protected) without failure.
* `lab2_list-4.png`: (corrected) average time per operation (for unprotected, mutex, and spin-lock) vs. number of threads.
* `README.txt`: A markdown readme file forced to have `.txt` extension.


## `make check` Methology
This option invokes the file `test.sh` to perform specified test cases.

## Note about GNUPlot
`Makefile` assumes that the machine has already installed GNUPlot 5.0 or newer. SEASNet server `lnxsrv09` installed it under `/usr/local/cs/bin`.


## Questions
### 2.1.1 Causing conflicts
Why does it take many iterations before errors are seen?
Why does a significantly smaller number of iterations so seldom fail
    
    The two questions have the same answer: with small iteration numbers, 
    threads can finish its work faster than a thread is created.

### 2.1.2 Cost of yielding

1. Why are the `--yield` runs so much slower? Where is the additional time going?
   
        When we run `--yield` and call `sched_yield()`, we put the current thread to the back of the queue, 
        and the OS will most likely choose to run another thread, which means a context switch. 
        The cost for context switch is very large.
   

2. Is it possible to get valid per-operation timings if we are using the `--yield` option? If so, explain how. If not, explain why not.
   
        No, since we are only collecting wall time, and multiple yield function may run at the same time. 
        We cannot collect the fraction of wall time devoted to context switches.
      

### 2.1.3 Measurement errors:

1. Why does the average cost per operation drop with increasing iterations?
    
        More iterations wipe away the overhead of creating a new thread. 
        In other words, overhead of creating a thread is amortized by a large number of iterations.
    

2. If the cost per iteration is a function of the number of iterations, how do we know how many iterations to run (or what the “correct” cost is)?
   
        If we look at the second plot, the average cost per iteration is decreasing exponentially. 
        Thus we will reach a point where the average cost per iteration does not decrease noticeably. 
        Thus when the curve is not decreasing significantly / becoming stable, 
        it should be a good estimate of the real time cost per operation.
   
### 2.1.4 Costs of serialization:
1. Why do all of the options perform similarly for low numbers of threads?

        Small number of threads means race condition / lock contention doesn't happen often. 
        Thus the fraction of time devoted to actual work (addition) will be similar.
   	
2. Why do the three protected operations slow down as the number of threads rises?

        Race condition / lock contention happens more often as the number of threads increases.

3. Why are spin-locks so expensive for large numbers of threads?

        Spin lock is polling and will waste a lot of CPU cycles. 
        Every function call to the lock variable has to be made as exclusive state in L1 cache. 
        It is very expensive.

### 2.2.1 Scalability of Mutex
Compare the variation in time per protected operation vs the number of threads (for mutex-protected operations) in Part-1 and Part-2, commenting on similarities/differences and offering explanations for them.

    Difference: Cost per operation trend
    The per operation cost of Part-2 grows much faster 
    since the list operation is significantly more expensive than a single addition.
    
    mutex: cost per operation
    Part-1 | Part-2
    ------ | ------
    ~20 - ~200 | ~200 - ~2000
    Note here I estimated the raw number and divide it by 4. The script incorrectly divided it by 16.
    As we can see the cost of list operatings is much higher since it is essentially `O(n)`.


### 2.2.2 Scalability of spin locks
Compare the variation in time per protected operation vs the number of threads for Mutex vs Spin locks, commenting on similarities/differences and offering explanations for them.

    Similarity: in part-2, the cost per operation of both locks grow as number of threads increase. 
    This is reasonable since more threads are competing for a single lock.
    
    Difference: 
    1. Actual cost: The cost of spin locks is lower when there aren't enough threads, 
                    but much higher when there are many threads competing for the lock.
                    
    2. Growth rate: the cost per operation for spin locks growth much faster than that of mutexes. 
    
    For mutex, threads are put to sleep when they are not run, so they don't waste CPU cycles.
    For spin locks, threads still need to run to check if they can get the lock 
                    and thus will waste a lot of CPU cycles.

