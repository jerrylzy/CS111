//
//  profile.c
//  Lab1
//
//  Created by Jerry Liu on 1/31/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <sys/resource.h>
#include <sys/time.h>
#include "profile.h"
#include "utility.h"

static void get_usage(int who, CPUTime_t *ctime) {
    struct rusage usage;
    if (getrusage(who, &usage) < 0)
        report_error("*** Error: getrusage() failed.\n");
    ctime->utime = usage.ru_utime;
    ctime->stime = usage.ru_stime;
}

void get_self_time(CPUTime_t *ctime) {
    get_usage(RUSAGE_SELF, ctime);
}

void get_child_time(CPUTime_t *ctime) {
    get_usage(RUSAGE_CHILDREN, ctime);
}

void get_time_diff(CPUTime_t *result, CPUTime_t *ctime) {
    CPUTime_t ctime_end;
    get_self_time(&ctime_end);
    timeval_subtract(&result->utime, &ctime_end.utime, &(ctime->utime));
    timeval_subtract(&result->stime, &ctime_end.stime, &(ctime->stime));
}

/*
 * GNU Example Code:
 * https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 * Subtract the ‘struct timeval’ values X and Y,
 * storing the result in RESULT.
 * Return 1 if the difference is negative, otherwise 0.
 */

static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }
    
    /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    
    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}
