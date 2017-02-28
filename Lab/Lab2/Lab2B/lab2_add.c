//
//  lab2_add.c
//  Lab2A
//
//  Created by Jerry Liu on 2/12/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OPT_THREADS     't'
#define OPT_ITERATIONS  'i'
#define OPT_SYNC        'n'
#define OPT_YIELD       'y'

#define UNPROTECTED     'u'
#define MUTEX           'm'
#define SPINLOCK        's'
#define CAS             'c'

static long long counter    = 0;
static int niterations      = 1;
static int opt_yield        = 0;
static int spinlock         = 0;
static char optsync         = UNPROTECTED;
static pthread_mutex_t lock;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void add_routine(int value) {
    //  Add 'value' #iteration times
    for (int i = 0; i < niterations; i++) {
        switch (optsync) {
            case UNPROTECTED:   add(&counter, value);   break;
            case MUTEX:
                pthread_mutex_lock(&lock);
                add(&counter, value);
                pthread_mutex_unlock(&lock);
                break;
            case SPINLOCK:
                while (__sync_lock_test_and_set(&spinlock, 1))
                    continue;
                add(&counter, value);
                __sync_lock_release(&spinlock);
                break;
            case CAS:
                for (long long oldval, newval; ; ) {
                    oldval = counter;
                    newval = oldval + value;
                    if (__sync_val_compare_and_swap(&counter, oldval, newval) == oldval)
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void* run_thread() {
    add_routine(1);
    add_routine(-1);
    return NULL;
}

int main(int argc, char *argv[]) {
    
    //  long options
    struct option long_options[] = {
        {"threads",     required_argument, 0, OPT_THREADS},
        {"iterations",  required_argument, 0, OPT_ITERATIONS},
        {"sync",        required_argument, 0, OPT_SYNC},
        {"yield",       no_argument,       0, OPT_YIELD},
        {0,             0,                 0, 0}
    };
    
    int ret;
    int nthreads = 1;
    char option[16] = "add";

    while ((ret = getopt_long(argc, argv, "", long_options, 0)) != -1) {
        switch (ret) {
            case OPT_THREADS:       nthreads = atoi(optarg);    break;
            case OPT_ITERATIONS:    niterations = atoi(optarg); break;
            case OPT_YIELD:         opt_yield = 1;              break;
            case OPT_SYNC:
                optsync = optarg[0];
                if (optsync != MUTEX && optsync != SPINLOCK && optsync != CAS) {
                    fprintf(stderr, "*** Error: wrong argument after --sync: %s\n", optarg);
                    exit(1);
                }
                if (optsync == MUTEX) {
                    if (pthread_mutex_init(&lock, NULL)) {
                        perror("*** Error: pthread_mutex_init failed.\n");
                        exit(1);
                    }
                }
                break;
            default:
                fprintf(stderr, "?? getopt returned unrecognized character code 0%o ??\n", ret);
                exit(-1);
                break;
        }
    }
    //  Format output
    if (opt_yield)
        strcat(option, "-yield");
    
    if (optsync == UNPROTECTED)
        strcat(option, "-none");
    else
        sprintf(option, "%s-%c", option, optsync);

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        perror("*** Error: malloc for pthread array failed.\n");
        exit(1);
    }
    //  Create N threads
    for (int i = 0; i < nthreads; i++) {
        if (pthread_create(&tid[i], NULL, run_thread, NULL)) {
            perror("*** Error: pthread_create failed.\n");
            free(tid);
            exit(1);
        }
    }
    
    //  Join N threads
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid[i], NULL)) {
            perror("*** Error: pthread_join failed.\n");
            exit(1);
        }
    }

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    //  High-res ellapsed time, in ns
    long long elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
    elapsed_time_ns += end_time.tv_nsec;
    elapsed_time_ns -= start_time.tv_nsec;
    
    //  Report data
    long long nops = nthreads * niterations * 2;
    //  option, # threads, # iterations, # operations, # run time,
    //  #avg time per op, counter value
    printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", option, nthreads, niterations, nops,
           elapsed_time_ns, elapsed_time_ns / nops, counter);
    free(tid);
    exit(counter != 0);
}
