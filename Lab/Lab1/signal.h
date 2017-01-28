//
//  signal.h
//  Lab1
//
//  Created by Jerry Liu on 1/27/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef signal_h
#define signal_h

//  Signal Macros
#define ABORT       '6'
#define CATCH       '7'
#define IGNORE      '8'
#define DEFAULT     '9'
#define PAUSE       'a'

void process_verbose(int option, int argc, char *argv[]);
void signal_handler (int signum);
void signal_process(int option);

#endif /* signal_h */
