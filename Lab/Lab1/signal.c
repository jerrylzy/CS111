//
//  signal.c
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "signal.h"
#include "utility.h"

void process_verbose(int option, int argc, char *argv[]) {
    int num_of_args = ! (option == ABORT || option == PAUSE);
    print_verbose(argc, num_of_args, argv);
}

void signal_handler (int signum) {
    fprintf(stderr, "Caught signal number: %d. Now exit ...\n", signum);
    exit(signum);
}

void signal_process(int option) {
    struct sigaction action;
    int signum;
    switch (option) {
        case ABORT:
            fprintf(stderr, "!!! Aborting program now. !!!\n");
            if (raise(SIGSEGV) != 0)
                report_error("*** Error: --raise error, cannot abort program.\n");
            break;
        case PAUSE:     pause();    break;
        default:
            signum = valid_integer(long_options[opt_idx].name, optarg);
            if (option == CATCH)
                action.sa_handler = signal_handler;
            else if (option == IGNORE)
                action.sa_handler = SIG_IGN;
            else
                action.sa_handler = SIG_DFL;
            if (sigaction(atoi(optarg), &action, NULL) == -1) {
                fprintf(stderr, "*** Error: --%s error, ", long_options[opt_idx].name);
                report_error("sigaction() failed.\n");
            }
            break;
    }
}
