#pragma once
#include "DHTlib/DHTlib.h"
// Declare functions
char* sconcat(const char* s1, const char* s2, const char* s3);
void btox(char* xp, const char* bb, int n);
void setupBoard();
bool readGPIO();
void loadPersist();
void savePersist();
void checkPersist();
bool readCard();

// Declare variables
int i2cFd;
char hexstr[8];
char mycard[8];
int relay1;
int relay2;
int relay3;
int pulsOk;
int pulsUp;
int pulsDown;
int cardHolder;
int pulsOkR;
int pulsUpR;
int pulsDownR;
int cardHolderR;
DHT_SensorData* pDHT;
float temp;
float hum;
int setpointTemp;
bool roomActive;
bool inMenu1;
bool inMenu2;
bool inMenuTemp;
bool inMenuRFID;
bool setup;

