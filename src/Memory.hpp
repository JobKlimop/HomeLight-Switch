#include <Arduino.h>
#include <EEPROM.h>

int getDeviceIdAddress();
int getPowerStateAddress();
int getGroupIdAddress();
int getDeviceNameAddress();
int getSsidAddress();
int getPasswordAddress();
void setSsid(String);
void setPassword(String);