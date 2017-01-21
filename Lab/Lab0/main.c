#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INPUT       'i'
#define OUTPUT      's'
#define SEGFAULT    'f'
#define CATCH       'c'
#define BUF_SIZE    2000

// SIGSEGV
void handler(int signum) {
    fprintf(stderr, "Segment fault. Signal number: %d\n", signum);
    exit(3);
}

int main(int argc, char *argv[]) {
    
    //  long options
    struct option long_options[] = {
        {"input",       required_argument, 0,  INPUT},
        {"output",      required_argument, 0,  OUTPUT},
        {"segfault",    no_argument,       0,  SEGFAULT},
        {"catch",    no_argument,       0,  CATCH},
        {0,             0,                 0,  0}
    };
    
    int fd0, fd1;
    ssize_t ret;
    char buf[BUF_SIZE+1];
    char *p = NULL;
    int segfault = 0;
    while ((ret = getopt_long(argc, argv, "", long_options, 0)) != -1) {
        // fprintf(stderr, "ret: %c, optindex: %d, optarg: %s\n", ret, optind, optarg);
        switch (ret) {
            case INPUT:
                if ((fd0 = open(optarg, O_RDONLY)) == -1) {
                    perror("Error opening input file");
                    exit(1);
                } else {
                    if (dup2(fd0, STDIN_FILENO) == -1) {
                        perror("Error redirecting output");
                        exit(-1);
                    }
                }
                break;
            case OUTPUT:
                if ((fd1 = creat(optarg, 0644)) == -1) {
                    perror("Error creating output file");
                    exit(2);
                } else {
                    if (dup2(fd1, STDOUT_FILENO) == -1) {
                        perror("Error redirecting output");
                        exit(-1);
                    }
                }
                break;
            case SEGFAULT:
                segfault = 1;
                break;
            case CATCH:
                signal(SIGSEGV, handler);
                break;
            default:
                fprintf(stderr, "?? getopt returned unrecognized character code 0%o ??\n", ret);
                exit(-1);
                break;
        }
    }
    
    if (segfault == 1)
        *p = 'A';
    
    while ((ret = read(STDIN_FILENO, &buf, BUF_SIZE)) > 0) {
        if (ret == -1) {
            perror("Error reading from stdin");
            exit(-1);
        }
        if (write(STDOUT_FILENO, &buf, ret) == -1) {
            perror("Error writing to stdout");
            exit(-1);
        }
    }
    exit(0);
}
