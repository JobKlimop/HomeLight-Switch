#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Memory.hpp>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Light.hpp>

extern AsyncWebServer server;
extern DynamicJsonDocument mqttJson;
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern String mqtt_topic;
extern String mqtt_message;
extern boolean new_message;

void initConnection();
void reconnectMqtt();
DynamicJsonDocument parseData(AsyncWebServerRequest *request, uint8_t *data, size_t jsonCapacity, String endpoint);
boolean sendData(String endpoint, String message);