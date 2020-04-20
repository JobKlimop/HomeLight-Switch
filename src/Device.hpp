#include <Arduino.h>
#include <EEPROM.h>
#include <Memory.hpp>

extern const int deviceIdAddress;
extern const int powerStateAddress;
extern const int groupNumberAddress;
extern const int deviceNameAddress;

int getDeviceId();
String getDeviceName();
int getGroupNr();

void setDeviceId(int id);
void setDeviceName(String deviceName);
void setGroup(int groupNr);
void softResetDevice();
void hardResetDevice();
void restartDevice();