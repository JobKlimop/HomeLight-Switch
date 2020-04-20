#include <Memory.hpp>

const int deviceIdAddress = 0;
const int powerStateAddress = 1;
const int groupNumberAddress = 2;
const int deviceNameAddress = 3;
const int ssidAddress = 13;
const int passwordAddress = 42;

int getDeviceIdAddress() {
    return deviceIdAddress;
}

int getPowerStateAddress() {
    return powerStateAddress;
}

int getGroupNumberAddress() {
    return groupNumberAddress;
}

int getDeviceNameAddress() {
    return deviceNameAddress;
}

int getSsidAddress() {
    return ssidAddress;
}

int getPasswordAddress() {
    return passwordAddress;
}