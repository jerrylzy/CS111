//
//  command.h
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef command_h
#define command_h

#include <sys/types.h>

//  Buffer size
#define BUF_SIZE    1000

typedef struct {
    char **cmd_argv;
    int    num_args;
} Args_t;

typedef struct {
    char   cmds[BUF_SIZE];
    pid_t  pid;
} Prog_t;

typedef struct {
    int      cmd_cap;
    int      cmd_idx;
    Prog_t **progs;
} Command_t;

extern Args_t *cmd_args;

Command_t *init_cmds_table();
static char **store_command (int argc, int required_argc, char *argv[]);
char **print_verbose(int argc, int required_argc, char *argv[]);
void add_command(Command_t **pcmds, Prog_t *prog);
void swap(Prog_t **lhs, Prog_t **rhs);

#endif /* command_h */
