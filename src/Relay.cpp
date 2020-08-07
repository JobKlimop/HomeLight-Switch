#include <Relay.hpp>

const int relayPin = 26;
int powerState;

void initPowerState() {
    pinMode(relayPin, OUTPUT);
    
    if(EEPROM.read(getPowerStateAddress()) == LOW || EEPROM.read(getPowerStateAddress()) == HIGH) {
    powerState = EEPROM.read(getPowerStateAddress());

    if(powerState == HIGH) {
        digitalWrite(relayPin, HIGH);
    } else {
        digitalWrite(relayPin, LOW);
    }
  } else {
      powerState = LOW;
      digitalWrite(relayPin, LOW);
      EEPROM.write(getPowerStateAddress(), powerState);
      EEPROM.commit();
  }
}

int getPowerState() {
    return EEPROM.read(getPowerStateAddress());
}

bool setPowerState(int state) {
    digitalWrite(relayPin, state);
    EEPROM.write(getPowerStateAddress(), state);
    EEPROM.commit();
    return true;
}