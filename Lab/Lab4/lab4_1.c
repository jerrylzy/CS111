//
//  lab4p1.c
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/12/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <mraa/aio.h>
#include <mraa/i2c.h>

#include "temp_sensor.h"

mraa_aio_context temp;
static sig_atomic_t volatile run_flag = true;

void handler(int signum) {
    fprintf(stderr, "Exit program, received signal: %d.\n", signum);
    run_flag = 0;
}

int main(void) {
    FILE *log_file = fopen("lab4_1.log", "w");
    if (log_file == NULL) {
        perror("*** Error: creating / open log file.\n");
        exit(EXIT_FAILURE);
    }
    
    temp = mraa_aio_init(0);
    signal(SIGINT, handler);

    setenv("TZ", "PST8PDT", true);
    tzset();
    
    while (run_flag) {
        read_write_temperature(log_file);
        sleep(1);
    }
    
    //  Close file
    if (fclose(log_file)) {
        perror("*** Error: cannot close log file.\n");
        exit(EXIT_FAILURE);
    }
    mraa_aio_close(temp);
    return 0;
}
