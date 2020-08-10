#include <Arduino.h>
#include <EEPROM.h>
#include <Memory.hpp>

String getDeviceId();
String getDeviceName();
String getGroupId();

bool setDeviceId(String id);
bool setDeviceName(String deviceName);
bool setGroup(String groupId);
void softResetDevice();
void hardResetDevice();
void restartDevice();