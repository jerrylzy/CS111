//
//  rgb_lcd.c
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//  Modified from Seeed library code
//

/*
 rgb_lcd.cpp
 2013 Copyright (c) Seeed Technology Inc.  All right reserved.
 
 Author:Loovee
 2013-9-18
 
 add rgb backlight fucnction @ 2013-10-15
 
 The MIT License (MIT)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.1  USA
*/

#include <string.h>
#include <mraa/i2c.h>

#include "rgb_lcd.h"

static unsigned char displayfunction;
static unsigned char displaycontrol;
static unsigned char displaymode;

void begin() {
    displayfunction |= LCD_2LINE;
    
    // turn the display on with no cursor or blinking default
    displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    
    // clear it off
    clear();
    
    // Initialize to default text direction (for romance languages)
    displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(LCD_ENTRYMODESET | displaymode);
    
    
    // backlight init
    set_reg(REG_MODE1, 0);
    // set LEDs controllable by both PWM and GRPPWM registers
    set_reg(REG_OUTPUT, 0xFF);
    // set MODE2 values
    // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
    set_reg(REG_MODE2, 0x20);
    
    //set_color_white();
    set_color_red();
}

/********** high level commands, for the user! */
void clear() {
    command(LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
}

void set_cursor(unsigned char col, unsigned char row) {
    char mask = (row << 7) >> 7;
    col |= ((mask & 0xc0) | (~mask & 0x80));
    unsigned char dta[2] = {0x80, col};
    mraa_i2c_write(lcd, dta, 2);
    
}

// Turn the display on/off (quickly)
void no_display() {
    set_color_all();
    displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | displaycontrol);
}

void display() {
    displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | displaycontrol);
}

/*********** mid level commands, for sending data/cmds */

//  write message
void write_msg(char *msg) {
    set_cursor(0, 0);
    while (*msg)
        write_byte(*msg++);
}

void set_reg(unsigned char addr, unsigned char dta)
{
    // transmit to device #4
    unsigned char data[2] = {addr, dta};
    mraa_i2c_write(rgb, data, 2);
    // stop transmitting
}

void set_rgb(unsigned char r, unsigned char g, unsigned char b)
{
    set_reg(REG_RED, r);
    set_reg(REG_GREEN, g);
    set_reg(REG_BLUE, b);
}

void set_color_all() {
    set_rgb(0, 0, 0);
}

void set_color_white() {
    set_rgb(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS);
}

void set_color_red() {
    set_rgb(BRIGHTNESS, 0, 0);
}
