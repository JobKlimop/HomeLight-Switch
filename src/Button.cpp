#include <Button.hpp>

const int buttonPin = 15;
long buttonTimer = 0;
long longButtonPressTime = 5000;

boolean buttonActive = false;
boolean longButtonPressActive = false;

void initButtonState() {
    pinMode(buttonPin, INPUT);
}

int checkButtonPress() {
    if(digitalRead(buttonPin) == HIGH) {
        if(buttonActive == false) {
            buttonActive = true;
            buttonTimer = millis();
        }

        if((millis() - buttonTimer > longButtonPressTime && (longButtonPressActive == false))) {
            longButtonPressActive = true;
            return 2;
        }
    } else {
        if(buttonActive == true) {
            if(longButtonPressActive == true) {
                longButtonPressActive = false;
            } else {
                return 1;
            }
            buttonActive = false;
        }
    }

    return 0;
}