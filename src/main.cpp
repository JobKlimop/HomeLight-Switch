#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
// #include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include <NeoPixelBus.h>
#include <AsyncJson.h>
// #include <ArduinoJson.h>

#define EEPROM_SIZE 1

const int powerStateAddress = 0;
const int groupNumberAddress = 1;

const uint16_t pixelCount = 82;
const uint pixelPin = 17;
const uint relayPin = 26;

const char* ssid = "DraadlozeWifiKabel";
const char* password = "GeefWifi!123";
AsyncWebServer server(80);

int powerState;
int groupNumber;
int colorSaturation = 255;
int greenValue = 0;
int redValue = 0;
int blueValue = 0;
int whiteValue = 0;

NeoPixelBus<NeoGrbwFeature, NeoEsp32I2s1800KbpsMethod> strip(pixelCount, pixelPin);
RgbwColor green(colorSaturation, 0, 0, 0);
RgbwColor red(0, colorSaturation, 0, 0);
RgbwColor blue(0, 0, colorSaturation, 0);
RgbwColor white(0, 0, 0, colorSaturation);
RgbwColor black(0);

void setGroupNumber(DynamicJsonDocument json) {
  groupNumber = json["groupNumber"];
  Serial.print("!");
  Serial.print(groupNumber);
  Serial.println("!");
  EEPROM.put(groupNumberAddress, groupNumber);
  Serial.println(EEPROM.commit());
  if(EEPROM.commit() == false) {
    Serial.println("False");
  } else {
    Serial.println("True");
  }
  EEPROM.commit();
}

DynamicJsonDocument parseData(AsyncWebServerRequest *request, uint8_t *data, String endpoint) {
  DynamicJsonDocument doc(2048);
  DeserializationError err = deserializeJson(doc, data);

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
    EEPROM.put(powerStateAddress, powerState);
    EEPROM.commit();
  }
    groupNumber = EEPROM.read(groupNumberAddress);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  Serial.println(WiFi.localIP());

  server.on("/controller/setGroup", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, 
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DynamicJsonDocument doc = parseData(request, data, "/setGroup");
      setGroupNumber(doc);
  });

  server.begin();
}

void loop() {
  Serial.println(powerState);
  Serial.println(EEPROM.get(powerStateAddress, powerState));
  Serial.println(groupNumber);
  Serial.println(EEPROM.get(groupNumberAddress, groupNumber));
  Serial.println("---------");
  delay(2000);
}