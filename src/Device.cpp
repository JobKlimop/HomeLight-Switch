#include <Device.hpp>

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
    int i = 13;
    int j = 42;
    setDeviceId("0");
    setDeviceName("");
    setGroup("0");
    EEPROM.write(getPowerStateAddress(), LOW);
    while(EEPROM.read(i) != 0x00) {
        EEPROM.write(i, 0x00);
        i++;
    }
    while(EEPROM.read(j) != 0x00) {
        EEPROM.write(j, 0x00);
        j++;
    }
    EEPROM.commit();

    ESP.restart();
}

void restartDevice() {
    ESP.restart();
}