#include "oled.h"
#include <math.h>
#include "sd1306.h"
#include "mt3620.h"
#include "main.h"

void oled_init()
{
	i2cFd = I2CMaster_Open(MT3620_ISU2_I2C);
	I2CMaster_SetBusSpeed(i2cFd, I2C_BUS_SPEED_STANDARD);
	I2CMaster_SetTimeout(i2cFd, 100);
	sd1306_init();
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "SETUP...", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
}

void oled_update_main() {
	char tempStr[16];
	char humStr[16];
	char roomStatus[16];
	sprintf(tempStr, "%4.2f", temp);
	sprintf(humStr, "%4.2f", hum);
	char* s1 = sconcat(tempStr, "°C", "");
	char* s2 = sconcat(humStr, "%", "");
	if (roomActive) {
		sprintf(roomStatus, "%s", "ENABLED");
	}else{
		sprintf(roomStatus, "%s", "DISABLED");
	}
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "RoomSphere", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_1_Y, s1, FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_2_Y, s2, FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_3_Y, roomStatus, FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
	free(s1);
	free(s2);
}

void oled_menu_1() {
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "TEMPERATURE", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_1_Y, "RFID CARD", FONT_SIZE_LINE, white_pixel);
	sd1306_refresh();
}
void oled_menu_2() {
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "TEMPERATURE", FONT_SIZE_LINE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_1_Y, "RFID CARD", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
}
void oled_menu_temp() {
	char setTempStr[16];
	sprintf(setTempStr, "%d", setpointTemp);
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "SETPOINT:", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_1_Y, setTempStr, FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_3_Y, "OK TO EXIT", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
}
void oled_menu_rfid() {
	char cardStr[16];
	sprintf(cardStr, "%s", mycard);
	clear_oled_buffer();
	sd1306_draw_string(0, OLED_LINE_0_Y, "ALLOWED:", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_1_Y, cardStr, FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_2_Y, "SCAN TO CHANGE", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(0, OLED_LINE_3_Y, "OK TO EXIT", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
}