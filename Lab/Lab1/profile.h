//
//  profile.h
//  Lab1
//
//  Created by Jerry Liu on 1/31/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef profile_h
#define profile_h

#include <sys/resource.h>
#include <sys/time.h>

typedef struct {
    struct timeval utime;
    struct timeval stime;
} CPUTime_t;

static void get_usage(int who, CPUTime_t *ctime);
void get_self_time(CPUTime_t *ctime);
void get_child_time(CPUTime_t *ctime);
void get_time_diff(CPUTime_t *result, CPUTime_t *ctime);
static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);

#endif /* profile_h */
