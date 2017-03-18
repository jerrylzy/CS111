//
//  rgb_lcd.h
//  Lab 4: Embedded Systems
//
//  Created by Jerry Liu on 3/16/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#ifndef __RGB_LCD_H__
#define __RGB_LCD_H__

#include <mraa/i2c.h>

// Device I2C Arress
#define LCD_ADDRESS     (0x7c>>1)
#define RGB_ADDRESS     (0xc4>>1)


// color define
#define WHITE           0
#define RED             1
#define GREEN           2
#define BLUE            3

#define REG_RED         0x04        // pwm2
#define REG_GREEN       0x03        // pwm1
#define REG_BLUE        0x02        // pwm0

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// brightness, 255 max
#define BRIGHTNESS 255

extern mraa_i2c_context lcd;
extern mraa_i2c_context rgb;

void begin();
void clear();
void no_display();
void display();
void write_msg(char *msg);

void set_cursor(unsigned char col, unsigned char row);

// color control
void set_rgb(unsigned char r, unsigned char g, unsigned char b);  // set rgb

void set_color_all();
void set_color_white();
void set_color_red();

void set_reg(unsigned char addr, unsigned char dta);

// send data
inline void write_byte(unsigned char value) {
    unsigned char dta[2] = {0x40, value};
    mraa_i2c_write(lcd, dta, 2);
}

// send command
inline void command(unsigned char value) {
    unsigned char dta[2] = {0x80, value};
    mraa_i2c_write(lcd, dta, 2);
}

#endif
