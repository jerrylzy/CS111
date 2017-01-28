//
//  utility.c
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "command.h"
#include "file.h"
#include "utility.h"

void report_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void cleanup(File_t *files, Command_t *cmds, Args_t *cmd_args) {
    close_file_table(files);
    free(files->fd_arr);
    free(files);
    free(cmds);
    free(cmd_args);
}

int valid_integer(const char *command, const char *argv) {
    int fd = atoi(argv);
    if ( ! strcmp(argv, "0") && fd != 0) {
        fprintf(stderr,
                "*** Error: --%s should be followed by 3 correct file numbers.\n",
                command);
        exit(1);
    }
    return fd;
}

void print_message(const char *option, const char *command, bool is_verbose) {
    //  buffer size 1000: assumption that passes test cases, not robust
    char buffer[BUF_SIZE];
    strcpy(buffer, option);
    strcat(buffer, command);
    if (is_verbose) {
        for (char **argv = cmd_args->cmd_argv, **end = argv + cmd_args->num_args;
             *argv != NULL && argv < end; argv++) {
            strcat(buffer, " ");
            strcat(buffer, *argv);
        }
    }
    strcat(buffer, "\n");
    
    //  Output the command line option
    if (write(STDOUT_FILENO, buffer, strlen(buffer)) == -1)
        report_error("*** Error: write error, cannot print command message.\n");
}

char **print_verbose(int argc, int required_argc, char *argv[]) {
    char **ret = store_command(argc, required_argc, argv);
    if (verbose_flag)  //  Print command line options
        print_message("--", long_options[opt_idx].name, true);
    return ret;
}

void print_exit_status(int status, const char *command) {
    char buffer[10];
    if (sprintf(buffer, "%d ", status) < 0)
        report_error("*** Error: sprintf fails to convert exit status to a string. \n");
    print_message(buffer, command, false);
}
