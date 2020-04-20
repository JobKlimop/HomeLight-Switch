#include <EEPROM.h>
#include <Connections.hpp>
#include <Device.hpp>
#include <Relay.hpp>
#include <Memory.hpp>
#include <Button.hpp>
#include <Light.hpp>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 64
#endif

boolean messageAlreadySent = false;
long messageTimer = 0;

void handleNewSettings() {
  //All topics
  if(mqtt_topic == "all/") {
    if(mqtt_message == "sdi"){
      String deviceJson = "{\"deviceId\":" + (String)getDeviceId() + 
                          ",\"deviceIp\":\"" + WiFi.localIP().toString() + "\"" +
                          ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                          ",\"groupNumber\":" + (String)getGroupNr() +
                          ",\"powerState\":" + (String)getPowerState() +
                          "}";
                          
      boolean messageReceived = sendData("/addNewDevice", deviceJson);

      if(messageReceived == true) {
        Serial.println("Message received");
      } else {
        Serial.println("Error");
      }
    }
  }

  //Device topics
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/di") {
    Serial.print("New device ID: ");
    Serial.println(mqtt_message.toInt());
    setDeviceId(mqtt_message.toInt());
  }
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/dn") {
    setDeviceName(mqtt_message);
  }
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/gn") {
    setGroup(mqtt_message.toInt());
  }
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/ps") {
    setPowerState(mqtt_message.toInt());
  }
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/rd") {
    if(mqtt_message == "true") {
      softResetDevice();
    }
  }
  if(mqtt_topic == "device/" + (String)getDeviceId() + "/hrd") {
    if(mqtt_message == "true") {
      hardResetDevice();
    }
  }

  if(mqtt_topic == "device/" + (String)getDeviceId() + "/res") {
    if(mqtt_message == "true") {
      restartDevice();
    }
  }

  //Group topics
  if(mqtt_topic == "group/" + (String)getGroupNr() + "/ps") {
    setPowerState(mqtt_message.toInt());
  }

  new_message = false;
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  EEPROM.begin(EEPROM_SIZE);

  initLight();
  setLedMode("BLINK");
  initPowerState();
  initConnection();
  initButtonState();

  Serial.print("DeviceID: ");
  Serial.println(getDeviceId());

  server.on("/controller/setNetworkCredentials", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 40;
    DynamicJsonDocument doc = parseData(request, data, jsonCapacity, "/setNetworkCredentials");

    String ssidString = doc["SSID"];
    String passwordString = doc["PASSWORD"];

    EEPROM.writeString(getSsidAddress(), ssidString);
    EEPROM.writeString(getPasswordAddress(), passwordString);
    EEPROM.commit();

    delay(10000);
    ESP.restart();
  });

  server.begin();
}

void loop() {
  int pressMode = checkButtonPress();
  if(pressMode == 1) {
    restartDevice();
  } else if(pressMode == 2) {
    Serial.println("{\"deviceId\":\"" + (String)getDeviceId() + "\"}");
    boolean messageReceived = sendData("/deleteDevice", "{\"deviceId\":\"" + (String)getDeviceId() + "\"}");
    
    if(messageReceived == true) {
      Serial.println("Message received");
      delay(500);
      hardResetDevice();
    } else {
      Serial.println("Error");
    }
  }

  if(WiFi.status() == WL_CONNECTED) {
    if(!mqttClient.connected()) {
      setLedMode("BLINK");
      reconnectMqtt();
    } else {
      setLedMode("OFF");
      if (getDeviceId() == 0) {
        if(messageAlreadySent == false){
        String deviceJson = "{\"deviceId\":" + (String)getDeviceId() + 
                            ",\"deviceIp\":\"" + WiFi.localIP() + "\"" +
                            ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                            ",\"groupNumber\":" + (String)getGroupNr() +
                            ",\"powerState\":" + (String)getPowerState() +
                            "}";

        boolean messageReceived = sendData("/addNewDevice", deviceJson);

        if(messageReceived == true) {
          Serial.println("Message received");
        } else {
          Serial.println("Error");
        }
        
        messageTimer = millis();
        messageAlreadySent = true;
        }

        if(millis() - messageTimer > 5000 && messageAlreadySent == true) {
          messageAlreadySent = false;
        }
      }
    }

    if(new_message == true) {
      handleNewSettings();
    }

    mqttClient.loop();
  } else {
    setLedMode("BLINK");
    initConnection();
  }
}