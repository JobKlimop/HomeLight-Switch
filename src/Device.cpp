#include <Device.hpp>
#include <Memory.hpp>

String getDeviceId() {
    return EEPROM.readString(getDeviceIdAddress());
}

String getDeviceName() {
    return EEPROM.readString(getDeviceNameAddress());
}

String getGroupId() {
    return EEPROM.readString(getGroupIdAddress());
}

bool setDeviceId(String id) {
    EEPROM.writeString(getDeviceIdAddress(), id);
    EEPROM.commit();
    return true;
}

bool setDeviceName(String deviceName) {
    EEPROM.writeString(getDeviceNameAddress(), deviceName);
    EEPROM.commit();
    return true;
}

bool setGroup(String groupId) {
    EEPROM.writeString(getGroupIdAddress(), groupId);
    EEPROM.commit();
    return true;
}

void softResetDevice() {
    setDeviceName("");
    setGroup("0");
    EEPROM.write(getPowerStateAddress(), LOW);

    ESP.restart();
}

void hardResetDevice() {
    int ssidAddress = getSsidAddress();
    int passwordAddress = getPasswordAddress();
    setDeviceId("0");
    setDeviceName("");
    setGroup("0");
    EEPROM.write(getPowerStateAddress(), LOW);
    while(EEPROM.read(ssidAddress) != 0x00 && ssidAddress < passwordAddress) {
        EEPROM.write(ssidAddress, 0x00);
        ssidAddress++;
    }
    while(EEPROM.read(passwordAddress) != 0x00) {
        EEPROM.write(passwordAddress, 0x00);
        passwordAddress++;
    }
    EEPROM.commit();
    delay(250);

    ESP.restart();
}

void restartDevice() {
    ESP.restart();
}