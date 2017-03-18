//
//  temp_sensor.c
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <mraa/aio.h>

#ifndef NO_DISP
#include "rgb_lcd.h"
#endif
#include "temp_sensor.h"

static const int B = 4275; // B value of the thermistor
sig_atomic_t volatile temp_type = FAHRENHEIT;
#ifndef NO_DISP
sig_atomic_t volatile disp      = true;  //  Default: display on
#endif

float read_write_temperature(FILE *log_file) {
    char outstr[BUF_SIZE];
#ifndef NO_DISP
    char display[BUF_SIZE];
#endif

    time_t t = time(NULL);
    //  not robust, removed conditional branches for performance
    struct tm *tm = localtime(&t);
    strftime(outstr, sizeof(outstr), "%H:%M:%S", tm);
    
    //  read temperature value
    uint16_t value = mraa_aio_read(temp);
    float R = 1023.0 / ((float) value) - 1.0;
    R = 100000.0 * R;
    //  convert to temperature via datasheet
    float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;
    if (temp_type == FAHRENHEIT)  //  Convert from C to F
        temperature = temperature * 1.8 + 32;

    //  print to stdout
    printf("%s %2.1f\n", outstr, temperature);

    //  print to log file
    fprintf(log_file, "%s %2.1f\n", outstr, temperature);

#ifndef NO_DISP
    //  display
    if (disp) {
        sprintf(display, "%2.1f%c", temperature, temp_type);
        write_msg(display);
    }
#endif
    return temperature;
}
