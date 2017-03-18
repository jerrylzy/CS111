//
//  test_driver.c
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <ctype.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <mraa/aio.h>
#include <mraa/i2c.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifdef TLS_TEST_DRIVER
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#endif

#include "rgb_lcd.h"
#include "temp_sensor.h"
#include "test_driver.h"

static const char UID[] = "404474229";
#define UID_SIZE 9   //  sizeof(UID)
#define MSG_SIZE 21  //  sizeof(socket_buf)

static sig_atomic_t volatile stop = false;
static sig_atomic_t volatile freq = 3;  //  Default T = 3

void *thread_send(void *arg) {
    Arg_t *parsed_arg = (Arg_t *) arg;
    FILE *log_file = parsed_arg->log_file;
#ifdef TLS_TEST_DRIVER
    SSL *ssl = parsed_arg->ssl;
#else
    int sockfd = parsed_arg->sockfd;
#endif
    char sock_buf[BUF_SIZE * 2];
    temp = mraa_aio_init(0);

    for ( ; ; ) {
        if ( ! stop) {
            //  critical section for reading temperature and writing logs
            float temperature = read_write_temperature(log_file);

            //  send to server, again not robust
            //  ######### TEMP = 50.9, len = 21
            sprintf(sock_buf, "%s TEMP = %2.1f", UID, temperature);
#ifdef TLS_TEST_DRIVER
            if (SSL_write(ssl, sock_buf, MSG_SIZE) < 0) {
                perror("*** Error: writing UID and temperature to server.\n");
                exit(EXIT_FAILURE);
            }
#else
            if (write(sockfd, sock_buf, MSG_SIZE) < 0) {
                perror("*** Error: writing UID and temperature to server.\n");
                exit(EXIT_FAILURE);
            }
#endif
            sleep(freq);  //  sleep for T = freq seconds
        }
    }
    return NULL;
}

inline
void log_cmd(const char *format, const char *cmd, FILE *log_file) {
    printf(format, cmd);
    fprintf(log_file, format, cmd);
    fflush(log_file);
}

inline
void log_invalid_cmd(const char *cmd, FILE *log_file) {
    log_cmd("%s I\n", cmd, log_file);
}

inline
void log_valid_cmd(const char *cmd, FILE *log_file) {
    log_cmd("%s\n", cmd, log_file);
}

void process_command(const char *cmd, FILE *log_file) {
    size_t len = strlen(cmd);
    if (len < MIN_CMD_LEN) {   //  saves some work
        log_invalid_cmd(cmd, log_file);
        return;
    }
    
    if (len == OFF_LEN && ! strncmp("OFF", cmd, OFF_LEN)) {
        log_valid_cmd(cmd, log_file);
        exit(EXIT_SUCCESS);  //  socket is closed automatically actually
    } else if (len == STOP_LEN && ! strncmp("STOP", cmd, STOP_LEN)) {
        //  alleviate race conditions between a STOP command and a reading
        sched_yield();
        stop = 1;
    } else if (len == START_LEN && ! strncmp("START", cmd, START_LEN)) {
        stop = 0;
    } else if ( ! strncmp("SCALE=", cmd, min(len, SCALE_LEN - 1))) {
        if (len != SCALE_LEN) {
            log_invalid_cmd(cmd, log_file);
            return;
        }
        char t = cmd[SCALE_LEN - 1];  //  type
        switch (t) {
            case CELSIUS:       temp_type = CELSIUS;            break;
            case FAHRENHEIT:    temp_type = FAHRENHEIT;         break;
            default:            log_invalid_cmd(cmd, log_file); return;
        }
    } else if ( ! strncmp("PERIOD=", cmd, min(len, MIN_PERIOD_LEN - 1))) {
        if (len < MIN_PERIOD_LEN || len > MAX_PERIOD_LEN) {
            log_invalid_cmd(cmd, log_file);
            return;
        }
        const char *temp_val = cmd + MIN_PERIOD_LEN - 1;
        
        //  Check if all characters are digits
        for (const char *iter = temp_val; *iter; iter++) {
            if ( ! isdigit(*iter)) {
                log_invalid_cmd(cmd, log_file);
                return;
            }
        }
        int ret = atoi(temp_val);
        if (ret < 1 || ret > 3600) {
            log_invalid_cmd(cmd, log_file);
            return;
        }
        freq = ret;
    } else if ( ! strncmp("DISP ", cmd, min(len, DISP_LEN - 1))) {
        if (len != DISP_LEN) {
            log_invalid_cmd(cmd, log_file);
            return;
        }
        char t = cmd[DISP_LEN - 1];
        switch (t) {
            case 'Y':
                disp = true;
                begin();
                break;
            case 'N':
                disp = false;
                no_display();
                break;
            default:    log_invalid_cmd(cmd, log_file); return;
        }
    } else {
        //  unrecognized or invalid command
        log_invalid_cmd(cmd, log_file);
        return;
    }
    //  log a valid entry
    log_valid_cmd(cmd, log_file);
}

#ifdef TLS_TEST_DRIVER
void init_ssl() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}
#endif

void close_resources(int dummy, void *arg) {
#ifdef TLS_TEST_DRIVER
    fprintf(stderr, "Closing SSL, file and socket with code %d\n", dummy);
#else
    fprintf(stderr, "Closing file and socket with code %d\n", dummy);
#endif
    Arg_t *parsed_arg = (Arg_t *) arg;
    FILE *log_file = parsed_arg->log_file;
    int sockfd = parsed_arg->sockfd;
#ifdef TLS_TEST_DRIVER
    SSL *ssl = parsed_arg->ssl;
    
    //  Close SSL
    SSL_shutdown(ssl);
    SSL_free(ssl);
#endif
    
    //  Close socket
    if (close(sockfd) < 0) {
        perror("*** Error: cannot close socket.\n");
        exit(EXIT_FAILURE);
    }
    
    //  Close file
    if (fclose(log_file)) {
        perror("*** Error: cannot close log file.\n");
        exit(EXIT_FAILURE);
    }
    no_display();
    mraa_i2c_stop(lcd);
    mraa_i2c_stop(rgb);
    mraa_aio_close(temp);
}

void client_process(const char *filename) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    struct hostent *server = gethostbyname(SERVER_NAME);
    if (server == NULL) {
        perror("gethostbyname error. host not available\n");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(PORT_NUM);
    
    //  Now connect to the server
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }
#ifndef TLS_TEST_DRIVER
    //  Connect to server
    if (write(sockfd, UID, UID_SIZE) < 0) {
        perror("*** Error: connecting to server with UID message.\n");
        exit(EXIT_FAILURE);
    }
#else
    //  Below code are referenced from http://fm4dd.com/openssl/sslconnect.htm
    
    //  Initialize SSL Context
    init_ssl();
    SSL_CTX *ctx = SSL_CTX_new(TLSv1_client_method());
    if (ctx == NULL) {
        perror("Unable to create a new SSL context structure.\n");
        exit(EXIT_FAILURE);
    }
    
    //  Create new SSL state object
    SSL *ssl = SSL_new(ctx);
    
    if ( ! SSL_set_fd(ssl, sockfd)) {
        perror("*** Error: OpenSSL set fd failed.\n");
        exit(EXIT_FAILURE);
    }
    
    if (SSL_connect(ssl) != 1) {
        perror("*** Error: OpenSSL connect failed.\n");
        exit(EXIT_FAILURE);
    }
    
    //  Connect to server by writing UID
    if (SSL_write(ssl, UID, UID_SIZE) < 0) {
        perror("*** Error: connecting to server with UID message.\n");
        exit(EXIT_FAILURE);
    }
#endif
    
    char buf[BUF_SIZE];
    sprintf(buf, "%s.log", filename);
    FILE *log_file = fopen(buf, "w");
    if (log_file == NULL) {
        perror("*** Error: creating / open log file.\n");
        exit(EXIT_FAILURE);
    }
    
    setenv("TZ", "PST8PDT", true);
    tzset();

    Arg_t arg;
    arg.log_file = log_file;
    arg.sockfd = sockfd;
#ifdef TLS_TEST_DRIVER
    arg.ssl = ssl;
#endif
    on_exit(close_resources, &arg);
    
    //  Init I2C bus for display
    mraa_init();
    
    rgb = mraa_i2c_init(0);
    lcd = mraa_i2c_init(0);
    
    mraa_i2c_address(lcd, LCD_ADDRESS);
    mraa_i2c_address(rgb, RGB_ADDRESS);
    begin();  //  start display
    
    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_send, &arg)) {
        perror("*** Error: pthread_create failed.\n");
        exit(EXIT_FAILURE);
    }
    
    //  Main thread receives commands from server
    for ( ; ; ) {
        memset(buf, 0, BUF_SIZE);
#ifdef TLS_TEST_DRIVER
        if (SSL_read(ssl, buf, BUF_SIZE - 1) < 0) {
            perror("*** Error: reading from server.\n");
            exit(EXIT_FAILURE);
        }
#else
        if (read(sockfd, buf, BUF_SIZE - 1) < 0) {
            perror("*** Error: reading from server.\n");
            exit(EXIT_FAILURE);
        }
#endif
        process_command(buf, log_file);
    }
}
