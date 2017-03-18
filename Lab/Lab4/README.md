# Lab 4: Embedded Systems
UID: 404474229
## File description
* `Makefile`: a Makefile that supports `dist, clean` options.
* `temp_sensor.?`: A module that integrates a sensor into the basic Edison platform and gathers readings from that sensor.
* `rgb_lcd.?`: A module that supports Grove LCD Display operations.
* `test_drive.?`: A module that can interact with both normal and secure remote server and performs all supported operations specified in the design for the application.
* `lab4_1.c`: A program that utilizes the temperature sensor module.
* `lab4_2.c`: A test run in which the program interacts with the remote server and performs all supported operations specified in the design for the application.
* `lab4_3.c`: A test run in which the SSL/TLS version of the program interacts with the secure remote server and performs all supported operations specified in the design for the application.
* `lab4_?.log`: log file from corresponding part
* `README.md`: A markdown readme file
* `s1.jpg`: Screenshot of DISP command.

## Display command
Part 2 and part 3 all support DISP command. The display should be connected to the `I2C` socket right next to `A0`.

## Makefile options
1. `make`

        builds the necessary program.

2. `make dist`

        builds the distribution tarball.

3. `make clean`

        cleans up any output file and already-built program.

## Note about student server compilation
If you want to run on the student server (address `localhost`), 
you can just remove the `#` near the end of the line for `CFLAGS`

## Usage
`make`, then `./lab4_1` for part 1, `./lab4_2` for part 2 and `./lab4_3` for part 3.

## Note about Edison board version
The edison board is flashed with the latest firmware `201606061707`.

## Note about part 1 temperature testing
I pressed my finger on the sensor for a while.

## Note about GNU Extension `on_exit()`
I used function `on_exit()` to close SSL and socket connection and log file before exit. GNU and (at least) edison's latest firmware support it. I don't need to close them manually since the OS will close them anyway, but this is merely for a better style. If there is anything wrong with `on_exit()`, the grader may simply remove any relevant code.

## Note about `sched_yield()`
This is to avoid race condition between a valid `STOP` command and an about-to-print temperature reading.
