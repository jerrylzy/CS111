//
//  temp_sensor.h
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef temp_sensor_h
#define temp_sensor_h

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

#include <mraa/aio.h>

#define BUF_SIZE 16

#define CELSIUS     'C'
#define FAHRENHEIT  'F'

extern mraa_aio_context temp;
extern sig_atomic_t volatile temp_type;
#ifndef NO_DISP
extern sig_atomic_t volatile disp;  //  Default: display on
#endif

float read_write_temperature(FILE *log_file);

#endif /* temp_sensor_h */
