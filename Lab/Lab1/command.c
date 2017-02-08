//
//  command.c
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"
#include "utility.h"

Command_t *init_cmds_table() {
    Command_t *cmds = malloc(sizeof(Command_t));
    if (cmds == NULL)
        report_error("*** Error: Can't allocate memory for commands table.\n");
    cmds->cmd_idx = 0;
    cmds->cmd_cap = 19;
    cmds->progs = malloc(sizeof(Prog_t *) * cmds->cmd_cap);
    if (cmds->progs == NULL)
        report_error("*** Error: Can't allocate memory for programs table.\n");
    return cmds;
}

static char **store_command (int argc, int required_argc, char *argv[]) {
    int index = optind;
    for ( ; index < argc; index++) {
        if ( ! strncmp(argv[index], "--", 2))
            break; //  Reached end of this argument
    }
    int args_length = strncmp(argv[optind - 1], "--", 2) ? index - optind + 1 : 0;
    if (args_length < required_argc) {
        fprintf(stderr,
                "*** Error: Lack arguments after --%s option. Arg number: %d. Need: %d\n",
                long_options[opt_idx].name, args_length, required_argc);
        exit(1);
    }
    cmd_args = malloc(sizeof(Args_t));
    char **cmd_argv = malloc(sizeof(char *) * (args_length + 1));
    if (cmd_args == NULL || cmd_argv == NULL)
        report_error("*** Error: malloc fails \
                     when creating command line arguments array.\n");
    cmd_args->cmd_argv = cmd_argv;
    cmd_args->num_args = args_length;
    
    //  Storing arguments
    if (required_argc) {
        cmd_argv[0] = optarg;
        for (int i = 1; i < args_length; i++)
            cmd_argv[i] = argv[optind + i - 1];
    }
    //  Last element is always NULL, easier for child processes to parse
    cmd_argv[args_length] = NULL;
    return cmd_argv;
}

char **print_verbose(int argc, int required_argc, char *argv[]) {
    char **ret = store_command(argc, required_argc, argv);
    if (verbose_flag)  //  Print command line options
        print_message("--", long_options[opt_idx].name, true);
    return ret;
}

void add_command(Command_t **pcmds, Prog_t *prog) {
    if (prog == NULL)
        report_error("*** Error: prog pointer is NULL.");
    Command_t *cmds;
    if (pcmds == NULL || (cmds = *pcmds) == NULL)
        report_error("*** Error: Command table doesn't exist.\n");
    if (cmds->cmd_idx == cmds->cmd_cap) {
        // Use a new pointer in case reallocation fails
        Command_t *newCmds = realloc(cmds, 2 * cmds->cmd_cap);
        if (newCmds == NULL)
            report_error("*** Error: Command table is too large to increase.\n");
        newCmds->cmd_cap *= 2;
        cmds = newCmds;
        *pcmds = cmds;
    }
    cmds->progs[cmds->cmd_idx++] = prog;
}

void swap(Prog_t **lhs, Prog_t **rhs) {
    Prog_t *temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
}
