//
//  utility.h
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef utility_h
#define utility_h

#include <getopt.h>
#include <stdbool.h>

#include "command.h"
#include "file.h"

//  File open-time flags
#define APPEND      O_APPEND
#define CLOEXEC     O_CLOEXEC
#define CREAT       O_CREAT
#define DIRECTORY   O_DIRECTORY
#define DSYNC       O_DSYNC
#define EXCL        O_EXCL
#define NOFOLLOW    O_NOFOLLOW
#define NONBLOCK    O_NONBLOCK
#define SYNC        O_SYNC
//#define RSYNC       O_RSYNC // On SEASNet this is O_SYNC
#define TRUNC       O_TRUNC

//  File-opening options
#define RDONLY      O_RDONLY
#define RDWR        O_RDWR
#define WRONLY      O_WRONLY
#define PIPE        '0'

//  Subcommand options
#define COMMAND     '1'
#define WAIT        '2'

//  Misc
#define CLOSE       '3'
#define VERBOSE     '4'
//#define PROFILE     '5'

#define MAX(a, b)   ((a) < (b) ? (b) : (a))

extern int           opt_idx;
extern bool          verbose_flag;
extern struct option long_options[];

void report_error(const char* msg);
void cleanup(File_t *files, Command_t *cmds, Args_t *cmd_args);
int valid_integer(const char *command, const char *argv);
void print_message(const char *option, const char *command, bool is_verbose);
char **print_verbose(int argc, int required_argc, char *argv[]);
void print_exit_status(int status, const char *command);


#endif /* utility_h */
