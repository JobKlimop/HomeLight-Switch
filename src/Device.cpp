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

void resetDeviceId() {
    int deviceIdAddress = getDeviceIdAddress();
    while(EEPROM.read(deviceIdAddress) != 0x00 && deviceIdAddress < getPowerStateAddress()) {
        EEPROM.write(deviceIdAddress, 0x00);
        deviceIdAddress++;
    }
    EEPROM.commit();
    setDeviceId("0");
}

void resetDeviceName() {
    int deviceNameAddress = getDeviceNameAddress();
    while(EEPROM.read(deviceNameAddress) != 0x00 && deviceNameAddress < getSsidAddress()) {
        EEPROM.write(deviceNameAddress, 0x00);
        deviceNameAddress++;
    }
    EEPROM.commit();
}

void resetSsid() {
    int ssidAddress = getSsidAddress();

    while(EEPROM.read(ssidAddress) != 0x00 && ssidAddress < getPasswordAddress()) {
        EEPROM.write(ssidAddress, 0x00);
        ssidAddress++;
    }
    EEPROM.commit();
}

void resetPassword() {
    int passwordAddress = getPasswordAddress();
    while(EEPROM.read(passwordAddress) != 0x00) {
        EEPROM.write(passwordAddress, 0x00);
        passwordAddress++;
    }
    EEPROM.commit();
}

void softResetDevice() {
    setDeviceName("");
    setGroup("0");
    EEPROM.write(getPowerStateAddress(), LOW);

    ESP.restart();
}

void hardResetDevice() {
    resetDeviceId();
    resetDeviceName();
    setGroup("0");
    EEPROM.write(getPowerStateAddress(), LOW);
    resetSsid();
    resetPassword();

    EEPROM.commit();
    delay(250);

    ESP.restart();
}

void restartDevice() {
    ESP.restart();
}