#pragma once

#ifndef OLED_H
#define OLED_H

#include <stdio.h>
#include <stdint.h>


#define OLED_NUM_SCREEN 7

#define OLED_TITLE_X      0
#define OLED_TITLE_Y      0 
#define OLED_RECT_TITLE_X 0
#define OLED_RECT_TITLE_Y 0
#define OLED_RECT_TITLE_W 127
#define OLED_RECT_TITLE_H 18

#define OLED_LINE_0_X     0
#define OLED_LINE_0_Y     0

#define OLED_LINE_1_X     0
#define OLED_LINE_1_Y     16

#define OLED_LINE_2_X     0
#define OLED_LINE_2_Y     32

#define OLED_LINE_3_X     0
#define OLED_LINE_3_Y     48


#define FONT_SIZE_TITLE   2
#define FONT_SIZE_LINE    1



const unsigned char Image_avnet_bmp[1024];

extern uint8_t oled_state;

int i2cFd;

extern void oled_init();
extern void oled_update_main();
extern void oled_menu_1();
extern void oled_menu_2();
extern void oled_menu_temp();
extern void oled_menu_rfid();


#endif
