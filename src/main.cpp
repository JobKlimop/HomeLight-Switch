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

String connectionStatus = "";
boolean messageAlreadySent = false;
long messageTimer = 0;

void handleNewSettings()
{
  //All topics
  if (mqtt_topic == "all/")
  {
    if (mqtt_message == "search_device" && new_message == true)
    {
      String deviceJson = "{\"_id\":" + (String)getDeviceId() +
                          ",\"deviceIp\":\"" + WiFi.localIP().toString() + "\"" +
                          ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                          ",\"groupId\":" + (String)getGroupId() +
                          ",\"powerState\":" + (String)getPowerState() +
                          "}";

      Serial.println("JSON: " + deviceJson);
      boolean messageReceived = sendData("/addNewDevice", deviceJson);
      Serial.print("messageReceived: ");
      Serial.println(messageReceived);

      if (messageReceived == 1)
      {
        Serial.println("Message received1");
        new_message = false;
      }
      else
      {
        Serial.println("Error");
      }
    }
  }

  //Device topics
  //Set the device ID
  if (mqtt_topic == "device/set_device_id")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String previous_id = doc["previous_id"];
    String _id = doc["_id"];

    Serial.println("Previous_id: " + previous_id);
    Serial.println("_id: " + _id);

    Serial.println("DeviceID: " + (String)getDeviceId());
    if ((String)getDeviceId() == (String)previous_id)
    {
      bool isSet = setDeviceId(_id);
      if (isSet == true)
      {
        publishMqttMessage("device/device_id_set", "{\"_id\":\"" + (String)getDeviceId() + "\" ,\"previousId\":\"" + previous_id + "\"}");
      }
    }
  }

  //Set name of the device
  if (mqtt_topic == "device/set_device_name")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String deviceName = doc["deviceName"];

    if ((String)getDeviceId() == (String)_id)
    {
      bool isSet = setDeviceName(deviceName);
      if (isSet == true)
      {
        publishMqttMessage("device/device_name_set", "{\"_id\":\"" + (String)getDeviceId() + "\",\"deviceName\":\"" + deviceName + "\"}");
      }
    }
  }

  //Set group number the device is in
  if (mqtt_topic == "device/set_group_number")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String groupId = doc["groupId"];

    if ((String)getDeviceId() == (String)_id)
    {
      bool isSet = setGroup(groupId);
      if (isSet == true)
      {
        publishMqttMessage("device/device_group_set", "{\"_id\":\"" + (String)getDeviceId() + "\",\"groupId\":\"" + groupId + "\"}");
      }
    }
  }

  //Set the device power state
  if (mqtt_topic == "device/set_power_state")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String powerState = doc["powerState"];

    if ((String)getDeviceId() == (String)_id)
    {
      bool powerStateSet = setPowerState(powerState.toInt());
      if (powerStateSet == true)
      {
        publishMqttMessage("device/power_state_set", "{\"_id\":\"" + (String)getDeviceId() + "\",\"powerState\":\"" + (String)getPowerState() + "\"}");
      }
    }
  }

  //Soft reset the device
  if (mqtt_topic == "device/reset_device")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String soft_reset = doc["soft_reset"];

    if ((String)getDeviceId() == (String)_id)
    {
      if (soft_reset == "true")
      {
        softResetDevice();
      }
    }
  }

  //Hard reset the device, which will delete all stored data
  if (mqtt_topic == "device/hard_reset_device")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String hard_reset = doc["hard_reset"];

    if ((String)getDeviceId() == (String)_id)
    {
      if (hard_reset == "true")
      {
        hardResetDevice();
      }
    }
  }

  //Restart the device
  if (mqtt_topic == "device/restart_device")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String _id = doc["_id"];
    String restart = doc["restart"];

    if ((String)getDeviceId() == (String)_id)
    {
      if (restart == "true")
      {
        restartDevice();
      }
    }
  }

  //Group topics
  //Sets powerstate for all devices within group
  if (mqtt_topic == "group/set_power_state")
  {
    const size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 80;
    DynamicJsonDocument doc(jsonCapacity);
    deserializeJson(doc, mqtt_message);
    String groupId = doc["groupId"];
    String powerState = doc["restart"];

    if ((String)getGroupId() == (String)groupId)
    {
      setPowerState(powerState.toInt());
      new_message = false;
    } 
  }
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
  connectionStatus = initConnection();
  initPowerState();
  initButtonState();

  Serial.print("DeviceID: ");
  Serial.println(getDeviceId());

  // Endpoint to send network credentials in setup mode
  server.on(
      "/controller/setNetworkCredentials", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        size_t jsonCapacity = JSON_OBJECT_SIZE(2) + 40;
        DynamicJsonDocument doc = parseData(request, data, jsonCapacity, "/setNetworkCredentials");

        String ssidString = doc["SSID"];
        String passwordString = doc["PASSWORD"];

        setSsid(ssidString);
        setPassword(passwordString);
        // EEPROM.writeString(getSsidAddress(), ssidString);
        // EEPROM.writeString(getPasswordAddress(), passwordString);
        // EEPROM.commit();

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
    Serial.println("{\"_id\":\"" + (String)getDeviceId() + "\"}");
    boolean messageReceived = sendData("/deleteDevice", "{\"_id\":\"" + (String)getDeviceId() + "\"}");

    if (messageReceived == true)
    {
      Serial.println("Message received2");
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
      if (getDeviceId() == "0")
      {
        if (messageAlreadySent == false)
        {
          String deviceJson = "{\"_id\":" + (String)getDeviceId() +
                              ",\"deviceIp\":\"" + WiFi.localIP().toString() + "\"" +
                              ",\"deviceName\":\"" + (String)getDeviceName() + "\"" +
                              ",\"groupId\":" + (String)getGroupId() +
                              ",\"powerState\":" + (String)getPowerState() +
                              "}";

          boolean messageReceived = sendData("/addNewDevice", deviceJson);

          if (messageReceived == true)
          {
            Serial.println("Message received3");
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
  else if (WiFi.status() != WL_CONNECTED && connectionStatus == "connect")
  {
    setLedMode("BLINK");
    initConnection();
  }
}