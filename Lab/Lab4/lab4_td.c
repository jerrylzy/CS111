//
//  lab4_td.c
//  For part 2 and part 3
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/12/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_driver.h"

#ifndef TLS_TEST_DRIVER
#define PROG_NAME "lab4_2"
#else
#define PROG_NAME "lab4_3"
#endif

int main(void) {
    client_process(PROG_NAME);
    return 0;
}
