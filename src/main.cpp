#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <NeoPixelBus.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#define EEPROM_SIZE 512

const int powerStateAddress = 0;
const int groupNumberAddress = 2;

const uint16_t pixelCount = 82;
const uint pixelPin = 17;
const uint relayPin = 26;

const char* ssid = "DraadlozeWifiKabel";
const char* password = "GeefWifi!123";
AsyncWebServer server(80);

int powerState;
int groupNumber;
int colorSaturation = 255;
int greenValue;
int redValue;
int blueValue;
int whiteValue;

NeoPixelBus<NeoGrbwFeature, NeoEsp32I2s1800KbpsMethod> strip(pixelCount, pixelPin);
RgbwColor green(colorSaturation, 0, 0, 0);
RgbwColor red(0, colorSaturation, 0, 0);
RgbwColor blue(0, 0, colorSaturation, 0);
RgbwColor white(0, 0, 0, colorSaturation);
RgbwColor black(0);

void setGroupNumber(DynamicJsonDocument json) {
  groupNumber = json["groupNumber"];
  EEPROM.write(groupNumberAddress, groupNumber);
  EEPROM.commit();
}

void setColor() {
  for(uint16_t pixel = 0; pixel < pixelCount; pixel++) {
    strip.SetPixelColor(pixel, RgbwColor(greenValue, redValue, blueValue, whiteValue));
  }
}

void setColorValues(DynamicJsonDocument json) {
  JsonArray colorValues = json["colorValues"];

  if(greenValue != colorValues[0]) {
    greenValue = colorValues[0];
  }
  if(redValue != colorValues[1]) {
    redValue = colorValues[1];
  }
  if(blueValue != colorValues[2]) {
    blueValue = colorValues[2];
  }
  if(whiteValue != colorValues[3]) {
    whiteValue = colorValues[3];
  }

  setColor();
}

void setBrightness(DynamicJsonDocument json) {
  colorSaturation = json["brightness"];
  setColor();
}

void setPowerState(DynamicJsonDocument json) {
  if(json["powerState"] == "HIGH") {
    digitalWrite(relayPin, HIGH);
    EEPROM.write(powerStateAddress, HIGH);
    EEPROM.commit();
    powerState = HIGH;
    setColorValues(json);
  } else if(json["powerState"] == "LOW") {
    digitalWrite(relayPin, LOW);
    EEPROM.write(powerStateAddress, LOW);
    EEPROM.commit();
    powerState = LOW;
  }
}

DynamicJsonDocument parseData(AsyncWebServerRequest *request, uint8_t *data, size_t jsonCapacity, String endpoint) {
  DynamicJsonDocument doc(jsonCapacity);
  DeserializationError err = deserializeJson(doc, data);

  Serial.println(serializeJsonPretty(doc, Serial));
  if(err) {
    request->send(400, "text/plain", "err on" + endpoint);
  } else {
    request->send(200, "application/json", "{'msg': 'OK'}");
  }

  return doc;
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  EEPROM.begin(EEPROM_SIZE);

  if(EEPROM.read(powerStateAddress) == LOW || EEPROM.read(powerStateAddress) == HIGH) {
    powerState = EEPROM.read(powerStateAddress);

    if(powerState == HIGH) {
      digitalWrite(relayPin, HIGH);
    }
  } else {
    powerState = LOW;
    EEPROM.write(powerStateAddress, powerState);
    EEPROM.commit();
  }
  if(EEPROM.read(groupNumberAddress) != 255) {
    groupNumber = EEPROM.read(groupNumberAddress);
  }

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  Serial.println(WiFi.localIP());

  server.on("/controller/setPowerState", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, 
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    size_t jsonCapacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc = parseData(request, data, jsonCapacity, "/setPowerState");
    setPowerState(doc);
  });

  server.on("/controller/setGroup", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, 
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    size_t jsonCapacity = JSON_OBJECT_SIZE(1);
      DynamicJsonDocument doc = parseData(request, data, jsonCapacity, "/setGroup");
      setGroupNumber(doc);
  });

  server.on("/controller/setColor", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, 
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    size_t jsonCapacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(4);
    DynamicJsonDocument doc = parseData(request, data, jsonCapacity, "/setColor");
    setColorValues(doc);
  });

  server.begin();
}

void loop() {
  Serial.println(powerState);
  Serial.println(EEPROM.read(powerStateAddress));
  Serial.println(groupNumber);
  Serial.println(EEPROM.read(groupNumberAddress));
  Serial.print("Brightness: ");
  Serial.println(colorSaturation);
  Serial.print("green: ");
  Serial.println(greenValue);
  Serial.print("red: ");
  Serial.println(redValue);
  Serial.print("blue: ");
  Serial.println(blueValue);
  Serial.print("white: ");
  Serial.println(whiteValue);
  Serial.println("---------");
  delay(2000);
}