#include <Connections.hpp>

const char* APSSID = "HomeLightAP";
const char* APPASSWORD = "HLPASS123!";
const char* mqttServer = "192.168.178.119";
const char* apiUrl = "http://192.168.178.60:3000";
const int mqttPort = 1883;
unsigned long lastMsg = 0;
String mqtt_topic = "";
String mqtt_message = "";
boolean new_message = false;
long mqtt_reconnect_timer;
boolean mqtt_reconnect_delay = false;

AsyncWebServer server(80);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
HTTPClient httpClient;

void setSubscriptions() {
    String all_topic = "all/#";
    String device_topic = "device/" + EEPROM.readString(getDeviceIdAddress()) + "/#";
    String group_topic = "group/" + (String)EEPROM.read(getGroupIdAddress()) + "/#";

    mqttClient.subscribe(all_topic.c_str());
    mqttClient.subscribe(device_topic.c_str());
    mqttClient.subscribe(group_topic.c_str());

    Serial.println("MQTT Connected!");
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.println("] ");
    mqtt_topic = (String)topic;
    mqtt_message = "";
    
    for(int i = 0; i < length; i++) {
        mqtt_message += (char)payload[i];
    }
    Serial.println(mqtt_message);
    new_message = true;
}

void connectMqtt() {
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);
}

void reconnectMqtt() {
    if(!mqttClient.connected() && mqtt_reconnect_delay == false) {
        Serial.println("Attempting MQTT connection...");
        String clientId = "device" + EEPROM.read(getDeviceIdAddress());
        clientId != String(random(0xffff), HEX);

        setLedMode("ON");
        if(mqttClient.connect(clientId.c_str())) {
            setSubscriptions();
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retry
            mqtt_reconnect_timer = millis();
            mqtt_reconnect_delay = true;
        }
    }

    if(millis() - mqtt_reconnect_timer > 5000 && mqtt_reconnect_delay == true) {
        mqtt_reconnect_delay = false;
    }
}

void wifiSetup() {
    WiFi.softAP(APSSID, APPASSWORD);
    Serial.println(WiFi.softAPIP());
}

void connectWiFi() {
    int connect_counter = 0;

    WiFi.begin(EEPROM.readString(getSsidAddress()).c_str(), EEPROM.readString(getPasswordAddress()).c_str());

    while(WiFi.status() != WL_CONNECTED) {
        setLedMode("BLINK");
        if(connect_counter == 60) {
            wifiSetup();
        }
        connect_counter++;
        Serial.print(".");
        delay(250);
    }

    Serial.println(WiFi.localIP());
    connectMqtt();
}

void initConnection() {
    if(EEPROM.readString(getSsidAddress()).length() > 0) {
        connectWiFi();
    } 
    else {
        wifiSetup();
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

boolean sendData(String endpoint, String message) {
    if(WiFi.status() == WL_CONNECTED) {
        httpClient.begin(apiUrl + endpoint);
        httpClient.addHeader("Content-Type", "application/json");
        int httpResponseCode = httpClient.POST(message);
        httpClient.end();
        Serial.println(httpResponseCode);

        if(httpResponseCode > 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}