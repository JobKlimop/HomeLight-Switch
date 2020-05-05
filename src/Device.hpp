#include <Arduino.h>
#include <EEPROM.h>
#include <Memory.hpp>

extern const int deviceIdAddress;
extern const int powerStateAddress;
extern const int groupIdAddress;
extern const int deviceNameAddress;

String getDeviceId();
String getDeviceName();
String getGroupId();

void setDeviceId(String id);
void setDeviceName(String deviceName);
void setGroup(String groupId);
void softResetDevice();
void hardResetDevice();
void restartDevice();