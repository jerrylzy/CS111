//
//  test_driver.h
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef test_driver_h
#define test_driver_h

#include <signal.h>

#ifdef TLS_TEST_DRIVER
#include <openssl/ssl.h>
#endif

#include <mraa/aio.h>
#include <mraa/i2c.h>

#define OFF_LEN         3
#define STOP_LEN        4
#define START_LEN       5
#define SCALE_LEN       7
#define MIN_PERIOD_LEN  8
#define MAX_PERIOD_LEN  11
#define DISP_LEN        6
#define MIN_CMD_LEN     OFF_LEN

#define READ_BUF_SIZE   256

//  Decide if this is run for TLS server
#ifdef TLS_TEST_DRIVER
#define PORT_NUM        17000
#else
#define PORT_NUM        16000
#endif

//  #define STUDENT_SERVER
#ifndef STUDENT_SERVER
#define SERVER_NAME "r01.cs.ucla.edu"
#else
#define SERVER_NAME "localhost"
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

typedef struct {
    FILE *log_file;
    int   sockfd;
#ifdef TLS_TEST_DRIVER
    SSL  *ssl;
#endif
} Arg_t;

mraa_i2c_context lcd;
mraa_i2c_context rgb;
mraa_aio_context temp;
extern sig_atomic_t volatile temp_type;
extern sig_atomic_t volatile disp;  //  Default: display on

void client_process(const char *filename);

#endif /* test_driver_h */
