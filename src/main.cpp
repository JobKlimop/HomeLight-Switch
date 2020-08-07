#include <EEPROM.h>
#include <Connections.hpp>
#include <Device.hpp>
#include <Relay.hpp>
#include <Memory.hpp>
#include <Button.hpp>
#include <Light.hpp>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 128
#endif

boolean messageAlreadySent = false;
long messageTimer = 0;

void handleNewSettings()
{
  //All topics
  if (mqtt_topic == "all/")
  {
    if (mqtt_message == "search_device")
    {
      String deviceJson = "{\"deviceId\":" + (String)getDeviceId() +
                          ",\"deviceIp\":\"" + WiFi.localIP().toString() + "\"" +
                          ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                          ",\"groupId\":" + (String)getGroupId() +
                          ",\"powerState\":" + (String)getPowerState() +
                          "}";

      boolean messageReceived = sendData("/addNewDevice", deviceJson);

      if (messageReceived == true)
      {
        Serial.println("Message received");
      }
      else
      {
        Serial.println("Error");
      }
    }
  }

  //Device topics
  //Set the device ID
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/set_device_id")
  {
    Serial.print("New device ID: ");
    // Serial.println(mqtt_message.toInt());
    setDeviceId(mqtt_message);
  }

  //Set name of the device
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/set_device_name")
  {
    setDeviceName(mqtt_message);
  }

  //Set group number the device is in
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/set_group_number")
  {
    setGroup(mqtt_message);
  }

  //Set the device power state
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/set_power_state")
  {
    setPowerState(mqtt_message.toInt());
  }

  //Soft reset the device
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/reset_device")
  {
    if (mqtt_message == "true")
    {
      softResetDevice();
    }
  }

  //Hard reset the device, which will delete all stored data
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/hard_reset_device")
  {
    if (mqtt_message == "true")
    {
      hardResetDevice();
    }
  }

  //Restart the device
  if (mqtt_topic == "device/" + (String)getDeviceId() + "/restart_device")
  {
    if (mqtt_message == "true")
    {
      restartDevice();
    }
  }

  //Group topics
  //Sets powerstate for all devices within group
  if (mqtt_topic == "group/" + (String)getGroupId() + "/set_power_state")
  {
    setPowerState(mqtt_message.toInt());
  }

  new_message = false;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  EEPROM.begin(EEPROM_SIZE);

  initLight();
  setLedMode("BLINK");
  initPowerState();
  initConnection();
  initButtonState();

  Serial.print("DeviceID: ");
  Serial.println(getDeviceId());

  server.on(
      "/controller/setNetworkCredentials", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
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

void loop()
{
  int pressMode = checkButtonPress();
  if (pressMode == 1)
  {
    restartDevice();
  }
  else if (pressMode == 2)
  {
    Serial.println("{\"deviceId\":\"" + (String)getDeviceId() + "\"}");
    boolean messageReceived = sendData("/deleteDevice", "{\"deviceId\":\"" + (String)getDeviceId() + "\"}");

    if (messageReceived == true)
    {
      Serial.println("Message received");
      delay(500);
      hardResetDevice();
    }
    else
    {
      Serial.println("Error");
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (!mqttClient.connected())
    {
      setLedMode("BLINK");
      reconnectMqtt();
    }
    else
    {
      setLedMode("OFF");
      if (getDeviceId() == 0)
      {
        if (messageAlreadySent == false)
        {
          String deviceJson = "{\"deviceId\":" + (String)getDeviceId() +
                              ",\"deviceIp\":\"" + WiFi.localIP() + "\"" +
                              ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                              ",\"groupId\":" + (String)getGroupId() +
                              ",\"powerState\":" + (String)getPowerState() +
                              "}";

          boolean messageReceived = sendData("/addNewDevice", deviceJson);

          if (messageReceived == true)
          {
            Serial.println("Message received");
          }
          else
          {
            Serial.println("Error");
          }

          messageTimer = millis();
          messageAlreadySent = true;
        }

        if (millis() - messageTimer > 5000 && messageAlreadySent == true)
        {
          messageAlreadySent = false;
        }
      }
    }

    if (new_message == true)
    {
      handleNewSettings();
    }

    mqttClient.loop();
  }
  else
  {
    setLedMode("BLINK");
    initConnection();
  }
}