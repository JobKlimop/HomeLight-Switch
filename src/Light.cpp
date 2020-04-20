#include <Light.hpp>

const int ledPin = 5;
String ledMode = "OFF";
long ledTimer = 500;
// boolean ledActive = false;
// boolean firstActivation = false;

void initLight() {
    pinMode(ledPin, OUTPUT);
    setLedMode("BLINK");
}

void turnLedOn() {
    digitalWrite(ledPin, HIGH);
}

void turnLedOff() {
    digitalWrite(ledPin, LOW);
}

void setLedMode(String mode) {
    ledMode = mode;
    static unsigned long ledTimer = 0;
    const long interval = 500;
    static bool ledActive = false;
    unsigned long now = millis();

    if(ledMode == "OFF") {
       turnLedOff();
    }

    if(ledMode == "ON") {
        turnLedOn();
    }

    if(ledMode == "BLINK") {
        Serial.print("ledMode: ");
        Serial.println(ledMode);
        Serial.print("ledActive: ");
        Serial.println(ledActive);
        Serial.print("ledTimer: ");
        Serial.println(ledTimer);
        if(now - ledTimer > interval && ledActive == false) {
            ledTimer = now;
            ledActive = !ledActive;
            turnLedOn();
        }
        
        if(now - ledTimer > interval && ledActive == true) {
            ledTimer = now;
            ledActive = !ledActive;
            turnLedOff();
        }
    }
}
