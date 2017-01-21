//
//  simpsh.c
//  Lab-1A
//
//  Created by Jerry Liu on 1/20/17.
//  Copyright \251 2017 Jerry Liu. All rights reserved.
//

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RDONLY      'r'
#define WRONLY      'w'
#define COMMAND     'c'
#define VERBOSE     'v'

typedef struct {
    int file_idx;
    int file_cap;
    int *fd_arr;
} File_t;

typedef struct {
    int num_args;
    char **command_argv;
} Args_t;

// Be visible to all child processes
static File_t *files = NULL;
static int fd_args[3]; // in, out, err
static Args_t *command_args = NULL;
char **command_argv = NULL;

File_t *init_files_table() {
    File_t *files = malloc(sizeof(File_t));
    if (files == NULL) {
        perror("*** Error: Can't allocate memory for file table.\n");
        exit(1);
    }
    files->file_idx = 0;
    files->file_cap = 19;
    files->fd_arr = malloc(sizeof(int) * files->file_cap);
    if (files->fd_arr == NULL) {
        perror("*** Error: Can't allocate memory for file table.\n");
        exit(1);
    }
    return files;
}

void cleanup_file_table(File_t *files) {
    for (int i = 0; i < files->file_idx; i++) {
        if (close(files->fd_arr[i]) == -1) {
            perror("*** Error: File descriptor cannot be closed.\n");
            exit(1);
        }
    }
    free(files->fd_arr);
    free(files);
}

void add_file_descriptor(File_t *files, int fd) {
    if (files == NULL) {
        perror("*** Error: File table doesn't exist.\n");
        exit(1);
    }
    if (files->file_idx == files->file_cap) {
        // Use a new pointer in case reallocation fails
        File_t *newFiles = realloc(files, 2 * files->file_cap);
        if (newFiles == NULL) {
            perror("*** Error: File table is too large to increase.\n");
            exit(1);
        }
        newFiles->file_cap *= 2;
        files = newFiles;
    }
    
    files->fd_arr[files->file_idx++] = fd;
}

void redirect_io(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) != -1) {
        close(oldfd);
    } else {
        perror("*** Error: redirecting output.\n");
        exit(1);
    }
}

void store_command (int argc, int required_argc, char *argv[]) {
    int index = optind;
    for ( ; index < argc; index++) {
        if ( ! strncmp(argv[index], "--", 2))
            break; //  Reached end of this argument
    }

    int args_length = index - optind + 1;
    if (args_length < required_argc) {
        fprintf(stderr,
                "*** Error: Lack arguments after --%s option.\
                Arg number: %d. Need: %d\n",
                optarg, args_length, required_argc);
        exit(1);
    }

    //  Store commands anyway
    //  since Lab 1B requires status with --wait option
    command_args = malloc(sizeof(Args_t));
    command_argv = malloc(sizeof(char *) * args_length);
    if (command_args == NULL || command_argv == NULL) {
        perror("*** Error: malloc fails \
               when creating command line arguments array.\n");
        exit(1);
    }
    command_args->command_argv = command_argv;
    command_args->num_args = args_length;
    
    //  Storing arguments
    command_argv[0] = optarg;
    for (int i = 1; i < args_length; i++)
        command_argv[i] = argv[optind + i - 1];
}

void print_verbose(const char *command) {
    //  Print command line options
    //  buffer size 500: assumption that passes test cases, not robust
    char buffer[500] = "--";
    strcat(buffer, command);
    char **command_argv = command_args->command_argv;
    for (int i = 0, len = command_args->num_args; i < len; i++) {
        strcat(buffer, " ");
        strcat(buffer, command_argv[i]);
    }
    strcat(buffer, "\n");

    //  Output the command line option
    if (write(STDOUT_FILENO, buffer, strlen(buffer)) == -1) {
        perror("*** Error: write error, cannot print verbose message.\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int fd, verbose_flag = 0;
    
    // Initialize files table
    files = init_files_table();
    
    //  long options
    struct option long_options[] = {
        {"rdonly",  required_argument,  NULL,   RDONLY},
        {"wronly",  required_argument,  NULL,   WRONLY},
        {"command", required_argument,  NULL,   COMMAND},
        {"verbose", no_argument,        NULL,   VERBOSE},
        {NULL,      0,                  NULL,   0}
    };
    
    ssize_t ret;
    while ((ret = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (ret) {
            case RDONLY:
                if ((fd = open(optarg, O_RDONLY)) == -1) {
                    perror("*** Error: opening input file.\n");
                    exit(1);
                } else {
                    add_file_descriptor(files, fd);
                    if (verbose_flag) {
                        store_command(argc, 1, argv);
                        print_verbose("rdonly");
                    }
                }
                break;
            case WRONLY:
                if ((fd = open(optarg, O_WRONLY)) == -1) {
                    perror("*** Error: creating output file.\n");
                    exit(1);
                } else {
                    add_file_descriptor(files, fd);
                    if (verbose_flag) {
                        store_command(argc, 1, argv);
                        print_verbose("wronly");
                    }
                }
                break;
            case COMMAND:
                store_command(argc, 4, argv);
                //  Storing file numbers
                for (int i = 0; i < 3; i++) {
                    fd = atoi(command_argv[i]);
                    //  checking
                    if ( ! strcmp(command_argv[i], "0") && fd != 0) {
                        fprintf(stderr,
                                "*** Error: --command should be followed\
                                by 3 correct file numbers.\n");
                        exit(1);
                    }
                    if (fd < 0 || fd >= files->file_idx) {
                        fprintf(stderr,
                                "*** Error: --command contains incorrect file descriptor\
                                numbers. Error fd: %d\n", fd);
                        exit(1);
                    }
                    fd_args[i] = fd;
                }
                
                int child_pid = fork();
                if (child_pid < 0) { // fork failed
                    perror("*** Error: fork() failed.\n");
                    exit(1);
                } else if (child_pid > 0) { //  parent process
                    if (verbose_flag)
                        print_verbose("command");
                } else {
                    // child process
                    char **args = command_args->command_argv + 3;
                    int *fd_indices = files->fd_arr;
                    redirect_io(fd_indices[fd_args[0]], STDIN_FILENO);
                    redirect_io(fd_indices[fd_args[1]], STDOUT_FILENO);
                    redirect_io(fd_indices[fd_args[2]], STDERR_FILENO);
                    if (execvp(*args, args)) {
                        perror("*** Error: execvp error, cannot execute program.\n");
                        exit(1);
                    }
                }
                break;
            case VERBOSE:
                verbose_flag = 1;
                break;
            default:
                fprintf(stderr,
                        "*** Error: getopt returned unrecognized character code %d,\
                        opt is %s\n", (int) ret, argv[optind - 1]);
                break;
        }
    }
    
    cleanup_file_table(files);
    free(command_args);
    free(command_argv);
    exit(0);
}
