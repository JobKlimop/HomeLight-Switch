#include <Memory.hpp>

const int deviceIdAddress = 0; //24 characters
const int powerStateAddress = 25; //1 character
const int groupIdAddress = 26; 
const int deviceNameAddress = 27; 
const int ssidAddress = 37;
const int passwordAddress = 66;

int getDeviceIdAddress() {
    return deviceIdAddress;
}

int getPowerStateAddress() {
    return powerStateAddress;
}

int getGroupIdAddress() {
    return groupIdAddress;
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

void setSsid(String ssid) {
    EEPROM.writeString(getSsidAddress(), ssid);
    EEPROM.commit();
}

void setPassword(String password) {
    EEPROM.writeString(getPasswordAddress(), password);
    EEPROM.commit();
}