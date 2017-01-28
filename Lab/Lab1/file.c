//
//  file.c
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file.h"
#include "utility.h"

File_t *init_files_table() {
    File_t *files = malloc(sizeof(File_t));
    if (files == NULL)
        report_error("*** Error: Can't allocate memory for file table.\n");
    files->file_idx = 0;
    files->file_cap = 19;
    files->fd_arr = malloc(sizeof(int) * files->file_cap);
    if (files->fd_arr == NULL)
        report_error("*** Error: Can't allocate memory for file table.\n");
    return files;
}

void close_file_table(File_t *files) {
    for (int i = 0; i < files->file_idx; i++) {
        if (files->fd_arr[i] == -1) {
            continue;
        } else if (close(files->fd_arr[i]) == -1) {
            report_error("*** Error: File descriptor cannot be closed.\n");
        } else {
            files->fd_arr[i] = -1;
        }
    }
}

void add_file_descriptor(File_t **pfiles, int fd) {
    File_t *files;
    if (pfiles == NULL || (files = *pfiles) == NULL)
        report_error("*** Error: File table doesn't exist.\n");
    if (files->file_idx == files->file_cap) {
        // Use a new pointer in case reallocation fails
        File_t *newFiles = realloc(files, 2 * files->file_cap);
        if (newFiles == NULL)
            report_error("*** Error: File table is too large to increase.\n");
        newFiles->file_cap *= 2;
        files = newFiles;
        *pfiles = files;
    }
    files->fd_arr[files->file_idx++] = fd;
}

void redirect_io(File_t *files, int pos_oldfd, int newfd) {
    int oldfd = files->fd_arr[pos_oldfd];
    if (dup2(oldfd, newfd) == -1)
        report_error("*** Error: redirecting output.\n");
    if (close(oldfd) == -1) {
        report_error("*** Error: closing file descriptors.\n");
    } else {
        files->fd_arr[pos_oldfd] = -1;
    }
}

int check_fd_args(File_t *files, const char *command, const char *argv) {
    int fd = valid_integer(command, argv);
    if (fd < 0 || fd >= files->file_idx) {
        fprintf(stderr,
                "*** Error: --%s contains incorrect file descriptor numbers.\
                Error fd: %d\n", command, fd);
        exit(1);
    }
    return fd;
}
