//
//  file.h
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef file_h
#define file_h

typedef struct {
    int *fd_arr;
    int  file_cap;
    int  file_idx;
} File_t;

File_t *init_files_table();
void add_file_descriptor(File_t **pfiles, int fd);
void close_file_table(File_t *files);
void redirect_io(File_t *files, int pos_oldfd, int newfd);
int check_fd_args(File_t *files, const char *command, const char *argv);

#endif /* file_h */
