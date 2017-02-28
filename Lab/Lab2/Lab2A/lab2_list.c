//
//  lab2_list.c
//  Lab2B
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

#include "SortedList.h"

//  Option flags
#define OPT_THREADS     't'
#define OPT_ITERATIONS  'i'
#define OPT_SYNC        'n'
#define OPT_YIELD       'y'

//  Protection flags
#define UNPROTECTED     'u'
#define MUTEX           'm'
#define SPINLOCK        's'

//  Yield flags
#define ID_YIELD        0x03  //  0x01 | 0x02
#define IL_YIELD        0x05  //  0x01 | 0x04
#define DL_YIELD        0x06  //  0x02 | 0x04
#define IDL_YIELD       0x07  //  0x01 | 0x02 | 0x04

//  Random key length
#define KEY_LEN         3

int opt_yield = 0;

static long long counter        = 0;
static int niterations          = 1;
static int nlists               = 1;
static unsigned int nelements   = 0;
static int spinlock             = 0;
static char optsync             = UNPROTECTED;

static pthread_mutex_t lock;
static SortedList_t list;
static SortedListElement_t *list_arr;
static int *start_poses;

void initialize(int nthreads) {
    srand((unsigned int) time(NULL));
    for (unsigned int i = 0; i < nelements; i++) {
        char *key = malloc(sizeof(char) * (KEY_LEN + 1));  //  This is not freed
        for (int i = 0; i < KEY_LEN; i++)
            key[i] = (char) rand() % 26 + 'A';  //  random ASCII chars
        key[KEY_LEN] = '\0';
        list_arr[i].key = key;
    }
    
    start_poses = malloc(sizeof(int) * nthreads);
    for (int i = 0, val = 0; i < nthreads; i++, val += niterations)
        start_poses[i] = val;
}

void check_delete(unsigned int pos) {
    SortedListElement_t *ele = SortedList_lookup(&list, list_arr[pos].key);
    //  Check for list corruption
    if (ele == NULL || SortedList_delete(ele) != 0) {
        fprintf(stderr, "*** Error: SortedList_lookup & delete: list is corrupted.\n");
        exit(EXIT_FAILURE);
    }
}

void* run_thread(void* pval) {
    //  Insert
    unsigned int start_pos = *((int *) pval);
    unsigned int upper_bound = start_pos + niterations;
    for (unsigned int i = start_pos; i < upper_bound; i++) {
        switch (optsync) {
            case UNPROTECTED:   SortedList_insert(&list, &list_arr[i]);   break;
            case MUTEX:
                pthread_mutex_lock(&lock);
                SortedList_insert(&list, &list_arr[i]);
                pthread_mutex_unlock(&lock);
                break;
            case SPINLOCK:
                while (__sync_lock_test_and_set(&spinlock, 1))
                    continue;  //  spining
                SortedList_insert(&list, &list_arr[i]);
                __sync_lock_release(&spinlock);
                break;
            default:
                break;
        }
    }
    
    //  List Length
    int ret = 0;
    switch (optsync) {
        case UNPROTECTED:   ret = SortedList_length(&list); break;
        case MUTEX:
            pthread_mutex_lock(&lock);
            ret = SortedList_length(&list);
            pthread_mutex_unlock(&lock);
            break;
        case SPINLOCK:
            while (__sync_lock_test_and_set(&spinlock, 1))
                continue;
            ret = SortedList_length(&list);
            __sync_lock_release(&spinlock);
            break;
        default:
            break;
    }
    if (ret < 0) {  //  Check for list corruption
        fprintf(stderr, "*** Error: SortedList_length: list is corrupted.\n");
        exit(EXIT_FAILURE);
    }

    //  Delete
    for (unsigned int i = start_pos; i < upper_bound; i++) {
        switch (optsync) {
            case UNPROTECTED:   check_delete(i);    break;
            case MUTEX:
                pthread_mutex_lock(&lock);
                check_delete(i);
                pthread_mutex_unlock(&lock);
                break;
            case SPINLOCK:
                while (__sync_lock_test_and_set(&spinlock, 1))
                    continue;  //  spining
                check_delete(i);
                __sync_lock_release(&spinlock);
                break;
            default:
                break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    //  long options
    struct option long_options[] = {
        {"threads",     required_argument, 0, OPT_THREADS},
        {"iterations",  required_argument, 0, OPT_ITERATIONS},
        {"sync",        required_argument, 0, OPT_SYNC},
        {"yield",       required_argument, 0, OPT_YIELD},
        {0,             0,                 0, 0}
    };
    
    int ret;
    int nthreads = 1;
    char option[32] = "list";
    size_t len;
    
    while ((ret = getopt_long(argc, argv, "", long_options, 0)) != -1) {
        switch (ret) {
            case OPT_THREADS:       nthreads = atoi(optarg);    break;
            case OPT_ITERATIONS:    niterations = atoi(optarg); break;
            case OPT_YIELD:
                len = strlen(optarg);
                if (len > 3) {
                    fprintf(stderr, "*** Error: wrong number of arguments for --yield: %lu\n", len);
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < len; i++) {  //  Set yield flags
                    if (optarg[i] == 'i') {
                        opt_yield |= INSERT_YIELD;
                    } else if (optarg[i] == 'd') {
                        opt_yield |= DELETE_YIELD;
                    } else if (optarg[i] == 'l') {
                        opt_yield |= LOOKUP_YIELD;
                    } else {
                        fprintf(stderr, "*** Error: wrong argument for --yield: %c\n", optarg[i]);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case OPT_SYNC:
                optsync = optarg[0];
                if (optsync != MUTEX && optsync != SPINLOCK) {
                    fprintf(stderr, "*** Error: wrong argument after --sync: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                if (optsync == MUTEX) {  //  Init lock
                    if (pthread_mutex_init(&lock, NULL)) {
                        perror("*** Error: pthread_mutex_init failed.\n");
                        exit(EXIT_FAILURE);
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
    switch (opt_yield) {
        case INSERT_YIELD:  strcat(option, "-i");       break;
        case DELETE_YIELD:  strcat(option, "-d");       break;
        case ID_YIELD:      strcat(option, "-id");      break;
        case LOOKUP_YIELD:  strcat(option, "-l");       break;
        case IL_YIELD:      strcat(option, "-il");      break;
        case DL_YIELD:      strcat(option, "-dl");      break;
        case IDL_YIELD:     strcat(option, "-idl");     break;
        default:            strcat(option, "-none");    break;
    }

    if (optsync == UNPROTECTED)
        strcat(option, "-none");
    else
        sprintf(option, "%s-%c", option, optsync);
    
    //  Initialize an empty list
    SortedList_t* head = &list;
    list.key = NULL;
    list.next = head;
    list.prev = head;

    //  Create an nthreads * niterations number of list elements
    nelements = nthreads * niterations;
    list_arr = malloc(sizeof(SortedListElement_t) * nelements);

    //  Generate keys
    initialize(nthreads);

    //  Record start wall time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        perror("*** Error: malloc for pthread array failed.\n");
        free(start_poses);
        free(list_arr);
        exit(EXIT_FAILURE);
    }
    
    //  Create N threads
    for (int i = 0, start_pos = 0; i < nthreads; i++, start_pos += niterations) {
        start_poses[i] = start_pos;
        if (pthread_create(&tid[i], NULL, run_thread, (void *) &start_poses[i])) {
            perror("*** Error: pthread_create failed.\n");
            free(tid);
            free(start_poses);
            free(list_arr);
            exit(EXIT_FAILURE);
        }
    }

    //  Join N threads
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid[i], NULL)) {
            perror("*** Error: pthread_join failed.\n");
            free(tid);
            free(start_poses);
            free(list_arr);
            exit(EXIT_FAILURE);
        }
    }

    //  Record end wall time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    //  Check final list length
    int listlen = SortedList_length(&list);
    if (listlen != 0) {
        if (listlen > 0)
            fprintf(stderr, "Final list length not zero, count: %d\n", listlen);
        else
            fprintf(stderr, "*** Error: SortedList_length: list is corrupted.\n");
        free(tid);
        free(start_poses);
        free(list_arr);
        exit(EXIT_FAILURE);
    }

    //  High-res ellapsed time, in ns
    long long elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
    elapsed_time_ns += end_time.tv_nsec;
    elapsed_time_ns -= start_time.tv_nsec;

    //  Report data
    //  This should be 2 since delete is O(1). Hope Dr. Reiher can fix this in the future.
    long long nops = nthreads * niterations * 3;
    //  option, # threads, # iterations, # lists, # operations, # run time,
    //  #avg time per op, list length
    printf("%s,%d,%d,%d,%lld,%lld,%lld\n", option, nthreads, niterations, nlists,
           nops, elapsed_time_ns, elapsed_time_ns / nops);
    free(tid);
    free(start_poses);
    free(list_arr);
    exit(EXIT_SUCCESS);
}
