//
//  simpsh.c
//  Lab 1: Simpleton Shell
//
//  Created by Jerry Liu on 1/20/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "command.h"
#include "file.h"
#include "signal.h"
#include "utility.h"

//  long options struct
struct option long_options[] = {
    //  File open-time flags
    {"append",      no_argument,        NULL,   APPEND},
    {"cloexec",     no_argument,        NULL,   CLOEXEC},
    {"creat",       no_argument,        NULL,   CREAT},
    {"directory",   no_argument,        NULL,   DIRECTORY},
    {"dsync",       no_argument,        NULL,   DSYNC},
    {"excl",        no_argument,        NULL,   EXCL},
    {"nofollow",    no_argument,        NULL,   NOFOLLOW},
    {"nonblock",    no_argument,        NULL,   NONBLOCK},
    {"rsync",       no_argument,        NULL,   SYNC},
    {"sync",        no_argument,        NULL,   SYNC},
    {"trunc",       no_argument,        NULL,   TRUNC},
    
    //  File-opening options
    {"rdonly",      required_argument,  NULL,   RDONLY},
    {"rdwr",        required_argument,  NULL,   RDWR},
    {"wronly",      required_argument,  NULL,   WRONLY},
    {"pipe",        no_argument,        NULL,   PIPE},
    
    //  Subcommand options
    {"command",     required_argument,  NULL,   COMMAND},
    {"wait",        no_argument,        NULL,   WAIT},
    
    //  Misc
    {"close",       required_argument,  NULL,   CLOSE},
    {"verbose",     no_argument,        NULL,   VERBOSE},
    //{"profile",     no_argument,        NULL,   PROFILE},  //  Lab 1C
    {"abort",       no_argument,        NULL,   ABORT},
    {"catch",       required_argument,  NULL,   CATCH},
    {"ignore",      required_argument,  NULL,   IGNORE},
    {"default",     required_argument,  NULL,   DEFAULT},
    {"pause",       no_argument,        NULL,   PAUSE},
    {NULL,          0,                  NULL,   0}
};

int  opt_idx      = 0;
bool verbose_flag = false;
Args_t *cmd_args  = NULL;

int main(int argc, char *argv[]) {
    bool wait_flag = false;
    int fd, file_flag = 0, exit_status = INT_MIN;
    int pipefd[2];
    int fd_args[3]; // in, out, err
    char **cmd_argv = NULL;
    File_t *files = init_files_table();
    Command_t *cmds = init_cmds_table();
    
    int ret;
    while ((ret = getopt_long(argc, argv, "", long_options, &opt_idx)) != -1) {
        switch (ret) {
                //  File open-time flags
            case APPEND:    case CLOEXEC:   case CREAT:     case DIRECTORY:
            case DSYNC:     case EXCL:      case NOFOLLOW:  case NONBLOCK:
            case SYNC:      case TRUNC:
                print_verbose(argc, 0, argv);
                file_flag |= ret;
                break;
                
                //  File-opening options
            case RDONLY: case WRONLY: case RDWR:
                print_verbose(argc, 1, argv);
                if ((fd = open(optarg, ret | file_flag, 0666)) == -1)
                    report_error("*** Error: opening file.\n");
                add_file_descriptor(&files, fd);
                file_flag = 0;
                break;
            case PIPE:
                print_verbose(argc, 0, argv);
                if (pipe(pipefd) == -1)
                    report_error("*** Error: creating a pipe.\n");
                add_file_descriptor(&files, pipefd[0]);  //  read end
                add_file_descriptor(&files, pipefd[1]);  //  write end
                break;
                
                //  Subcommand options
            case COMMAND:
                cmd_argv = print_verbose(argc, 4, argv);
                for (int i = 0; i < 3; i++)   //  Storing file numbers
                    fd_args[i] = check_fd_args(files, "command", cmd_argv[i]);;
                
                int child_pid = fork();
                if (child_pid < 0) { // fork failed
                    report_error("*** Error: fork() failed.\n");
                } else if (child_pid == 0) {
                    //  child process
                    char **args = cmd_args->cmd_argv + 3;
                    redirect_io(files, fd_args[0], STDIN_FILENO);
                    redirect_io(files, fd_args[1], STDOUT_FILENO);
                    redirect_io(files, fd_args[2], STDERR_FILENO);
                    close_file_table(files);
                    execvp(*args, args); //  if it succeeds, it won't return.
                    free(args);
                    perror("*** Error: execvp error, cannot execute program.\n");
                    _exit(EXIT_FAILURE);  //  child process, use _exit instead
                } else { //  parent process
                    //  store program information
                    char **args = cmd_args->cmd_argv + 3;
                    int arglen = cmd_args->num_args - 3;
                    Prog_t *prog = malloc(sizeof(Prog_t));
                    if (prog == NULL)
                        report_error("*** Error: cannot allocate memory for Prog_t.\n");
                    strcpy(prog->cmds, args[0]);
                    for (int i = 1; i < arglen; i++) {
                        strcat(prog->cmds, " ");
                        strcat(prog->cmds, args[i]);
                    }
                    prog->pid = child_pid;
                    add_command(&cmds, prog);
                }
                break;
            case WAIT:
                wait_flag = true;
                print_verbose(argc, 0, argv);
                close_file_table(files);
                int wstatus;
                Prog_t *prog = NULL;
                pid_t cpid;
                while (cmds->cmd_idx) {
                    if ((cpid = waitpid(-1, &wstatus, 0)) < 0)
                        report_error("*** Error: waitpid() fails.\n");
                    for (int i = 0; i < cmds->cmd_idx; i++) {
                        prog = cmds->progs[i];
                        if (prog->pid != cpid)
                            continue;
                        int status = WEXITSTATUS(wstatus);
                        exit_status = MAX(exit_status, status);
                        print_exit_status(status, prog->cmds);
                        //  remove an element from command table
                        free(prog);
                        cmds->progs[i] = NULL;
                        swap(cmds->progs + i, cmds->progs + (--cmds->cmd_idx));
                    }
                }
                break;
                
                //  Misc
            case CLOSE:
                print_verbose(argc, 1, argv);
                check_fd_args(files, "close", optarg);
                if (close(files->fd_arr[atoi(optarg)]) == -1) {
                    report_error("*** Error: closing file descriptors.\n");
                } else {
                    files->fd_arr[atoi(optarg)] = -1; // indicate fd's already closed
                }
                break;
            case VERBOSE:   verbose_flag = true;   break;
            case ABORT: case CATCH: case IGNORE: case DEFAULT: case PAUSE:
                process_verbose(ret, argc, argv);
                signal_process(ret);
                break;
            default:
                fprintf(stderr, "*** Error: unrecognized option: --%s\n", long_options[opt_idx].name);
                break;
        }
    }
    cleanup(files, cmds, cmd_args);
    exit(wait_flag ? exit_status : EXIT_SUCCESS);
}
