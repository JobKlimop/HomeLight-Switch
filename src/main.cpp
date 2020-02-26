#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_POWER_STATE_SIZE 1

const int powerStateAddress = 0;
int powerState;

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  // put your setup code here, to run once:
  Serial.println("Time for setup()");

  EEPROM.begin(EEPROM_POWER_STATE_SIZE);

  Serial.println("powerstate1: " + powerState);
  Serial.println(EEPROM.read(powerStateAddress));

  if(EEPROM.read(powerStateAddress) == 0 || EEPROM.read(powerStateAddress) == 1) {
    powerState = EEPROM.read(powerStateAddress);
  } else {
    EEPROM.write(powerStateAddress, LOW);
    powerState = EEPROM.read(powerStateAddress);
  }

  Serial.println("powerstate2: " + powerState);
  Serial.println(EEPROM.read(powerStateAddress));

  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Time for loop()");

  Serial.println(powerState);
  Serial.println(EEPROM.read(powerStateAddress));
  delay(2000);
}