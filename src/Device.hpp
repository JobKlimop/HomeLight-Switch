#include <Arduino.h>
#include <EEPROM.h>
#include <Memory.hpp>

String getDeviceId();
String getDeviceName();
String getGroupId();

void setDeviceId(String id);
void setDeviceName(String deviceName);
void setGroup(String groupId);
void softResetDevice();
void hardResetDevice();
void restartDevice();