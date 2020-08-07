#include <Arduino.h>
#include <EEPROM.h>
#include <Memory.hpp>

extern int powerState;

void initPowerState();
int getPowerState();
bool setPowerState(int state);