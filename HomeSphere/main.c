#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <applibs/gpio.h>
#include <applibs/storage.h>
#include "mfrc522.h"
#include "main.h"
#include "sd1306.h"
#include "oled.h"
#include "mt3620.h"
#include "DHTlib/DHTlib.h"


// Declare variables
const struct timespec sleepLoop = { 2, 0 };
const struct timespec sleepDoor = { 4, 0 };


int main(void)
{
    // Setup the board and external components
	setupBoard();

    while (true) {
		// Check RFID tag
		if (readCard()) {
			// Check if readed tag is the same hardcoded below
			if (strcmp(hexstr, mycard) == 0) {
				Log_Debug("[INFO] Opening door!");
				GPIO_SetValue(relay3, GPIO_Value_High);
				nanosleep(&sleepDoor, NULL);
				GPIO_SetValue(relay3, GPIO_Value_Low);
			}
			// If we are in the menu, save the card as authorized
			if (inMenuRFID) {
				sprintf(mycard, "%s", hexstr);
				savePersist();
				oled_menu_rfid();
			}
		}
		// Read GPIO
		readGPIO();
		// Update OLED main page if we are not in setup
		if (!setup) {
			oled_update_main();
		}
		// Wait a couple of seconds
        nanosleep(&sleepLoop, NULL);
    }
}


void setupBoard() {
	Log_Debug("Starting RoomSphere v1.0\n");
	// Init RFID card reader
	mfrc522_init();
	// Init I2C and OLED display
	oled_init();
	// Check if mutable storage is already filled, otherwhise set defaults
	checkPersist();
	loadPersist();
	// Setup GPIOs
	pulsUp = GPIO_OpenAsInput(42);
	pulsDown = GPIO_OpenAsInput(43);
	pulsOk = GPIO_OpenAsInput(26);
	cardHolder = GPIO_OpenAsInput(17);
	relay1 = GPIO_OpenAsOutput(2, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	relay2 = GPIO_OpenAsOutput(28, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	relay3 = GPIO_OpenAsOutput(1, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	Log_Debug("---Ready---\n\n");
}

bool readGPIO() {
	// Read GPIO inputs
	GPIO_GetValue(pulsOk, &pulsOkR);
	GPIO_GetValue(pulsUp, &pulsUpR);
	GPIO_GetValue(pulsDown, &pulsDownR);
	GPIO_GetValue(cardHolder, &cardHolderR);
	Log_Debug("[GPIO] Buttons status: %d, %d, %d, %d\n", pulsOkR, pulsUpR, pulsDownR, cardHolderR);
	// Read DHT11 data
	pDHT = DHT_ReadData(0);
	if (pDHT != NULL) {
		temp = pDHT->TemperatureCelsius;
		hum = pDHT->Humidity;
	}
	// If required temperature is higher, turn ON heating relay
	if (temp < (float)setpointTemp) {
		GPIO_SetValue(relay2, GPIO_Value_High);
	}else{
		GPIO_SetValue(relay2, GPIO_Value_Low);
	}
	// If a card is fitted inside the card holder activate room power
	if (cardHolderR == 0) {
		roomActive = true;
		GPIO_SetValue(relay1, GPIO_Value_High);
	}else{
		roomActive = false;
		GPIO_SetValue(relay1, GPIO_Value_Low);
	}
	// Check buttons and take actions
	if (pulsOkR == 1){
		if (!setup){
			inMenu1 = true;
			oled_menu_1();
			setup = true;
			return true;
		}
		if (inMenu1) {
			inMenu1 = false;
			inMenuTemp = true;
			oled_menu_temp();
			return true;
		}
		if (inMenu2) {
			inMenu2 = false;
			inMenuRFID = true;
			oled_menu_rfid();
			return true;
		}
		if (inMenuTemp) {
			inMenuTemp = false;
			setup = false;
			oled_update_main();
			savePersist();
			return true;
		}
		if (inMenuRFID) {
			inMenuRFID = false;
			setup = false;
			oled_update_main();
			return true;
		}
	}
	if (pulsDownR == 1) {
		if (inMenu1) {
			inMenu1 = false;
			inMenu2 = true;
			oled_menu_2();
			return true;
		}
		if (inMenu2) {
			inMenu2 = false;
			inMenu1 = true;
			oled_menu_1();
			return true;
		}
		if (inMenuTemp) {
			setpointTemp = setpointTemp - 1;
			oled_menu_temp();
			return true;
		}
	}
	if (pulsUpR == 1) {
		if (inMenu2) {
			inMenu2 = false;
			inMenu1 = true;
			oled_menu_1();
			return true;
		}
		if (inMenu1) {
			inMenu1 = false;
			inMenu2 = true;
			oled_menu_2();
			return true;
		}
		if (inMenuTemp) {
			setpointTemp = setpointTemp + 1;
			oled_menu_temp();
			return true;
		}
	}
	
	
}

bool readCard() {
	uint8_t str[MAX_LEN];
	uint8_t byte = mfrc522_request(PICC_REQALL, str);
	if (byte == CARD_FOUND) {
		Log_Debug("[MFRC522][INFO] Found a card: %x\n", byte);
		byte = mfrc522_get_card_serial(str);
		if (byte == CARD_FOUND)
		{
			btox(hexstr, str, 8);
			hexstr[8] = 0;
			Log_Debug("[MFRC522][INFO] Serial: %s\n", hexstr);
			return true;
		}
	}
	return false;
}

void btox(char* xp, const char* bb, int n){
	const char xx[] = "0123456789ABCDEF";
	while (--n >= 0) xp[n] = xx[(bb[n >> 1] >> ((1 - (n & 1)) << 2)) & 0xF];
}

void checkPersist() {
	// Check if a valid config already exist...
	int fd = Storage_OpenMutableFile();
	char buf[16];
	ssize_t ret = read(fd, buf, 12);
	close(fd);
	char* pPosition = strchr(buf, '-');
	// ... otherwise save defaults
	if (pPosition == NULL) {
		setpointTemp = 20;
		sprintf(mycard, "%s", "FFFFFFFF");
		Log_Debug("[STORAGE]Memory first init. Writing defaults.\n");
		savePersist();
	}
}

void loadPersist() {
	// Load temperature setpoint and RFID tag
	int fd = Storage_OpenMutableFile();
	char buf[16];
	ssize_t ret = read(fd, buf, 12);
	Log_Debug("[STORAGE]Readed data as follow: %s\n", buf);
	char delim[] = "-";
	char* token = strtok(buf, delim);
	setpointTemp = atoi(token);
	Log_Debug("[STORAGE]TEMP: %d\n", setpointTemp);
	token = strtok(NULL, delim);
	sprintf(mycard, "%s", token);
	Log_Debug("[STORAGE]RFID: %s\n", mycard);
	close(fd);
}

void savePersist() {
	// Save temperature setpoint and RFID tag
	int rt = Storage_DeleteMutableFile();
	int fd = Storage_OpenMutableFile();
	char tempStr[16];
	sprintf(tempStr, "%d", setpointTemp);
	char* s = sconcat(tempStr, "-", mycard);
	ssize_t ret = write(fd, s, 12);
	Log_Debug("[STORAGE]Written data as follow: %s\n", s);
	free(s);
	close(fd);
}

char* sconcat(const char* s1, const char* s2, const char* s3){
	char* result = malloc(strlen(s1) + strlen(s2)+ strlen(s3) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	strcat(result, s3);
	return result;
}

