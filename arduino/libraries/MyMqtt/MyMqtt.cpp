/*
    File: MyMqtt.cpp
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Create connection with homeassistant through MQTT protocol
*/

#include "MyMqtt.h"

MyMqtt::MyMqtt()
{
    mqttName = "Curtains";
    sensorsStateTopic = "home/curtains/sensors/state";
    positionTopic = "home/curtains/position";
    lightModeTopic = "home/curtains/modes/light";
    timeModeTopic = "home/curtains/modes/time";
    timetableTopic = "home/curtains/timetable";
    openThresholdTopic = "home/curtains/light/open_treshold";
    closeThresholdTopic = "home/curtains/light/close_treshold";
    logoutTopic = "home/curtains/disconnect";
}

void MyMqtt::sendMQTTCurtainsDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/cover/curtains/position/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["name"] = "Curtains Cover";
    doc["cmd_t"] = positionTopic;
    doc["dev_cla"] = "curtain";
    doc["pos_t"] = positionTopic;
    doc["set_pos_t"] = positionTopic;
    doc["ret"] = true;
    doc["pl_open"] = 0;
    doc["pl_cls"] = 100;
    doc["pos_open"] = 0;
    doc["pos_clsd"] = 100;

    size_t n = serializeJson(doc, buffer);
    bool publishedVerify = client.beginPublish(discoveryTopic, n, true);
    publishedVerify = client.write((const uint8_t *)buffer, 150);
    publishedVerify = client.write((const uint8_t *)buffer + 150, n - 150);
    publishedVerify = client.endPublish();

    if (publishedVerify)
    {
        Serial.println("Curtains discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending curtains discovery message failed");
    }
}

void MyMqtt::sendMQTTTemperatureDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/sensor/curtains/temperature/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["name"] = "Motor temperature";
    doc["stat_t"] = sensorsStateTopic;
    doc["unit_of_meas"] = "°C";
    doc["dev_cla"] = "temperature";
    doc["frc_upd"] = true;
    doc["ret"] = true;
    doc["val_tpl"] = "{{ value_json.temperature | round(1) }}";

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Temperature discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending temperature discovery message failed");
    }
}

void MyMqtt::sendMQTTLightDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/sensor/curtains/light/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["name"] = "Outside light";
    doc["stat_t"] = sensorsStateTopic;
    doc["unit_of_meas"] = "lx";
    doc["dev_cla"] = "illuminance";
    doc["frc_upd"] = true;
    doc["ret"] = true;
    doc["val_tpl"] = "{{ value_json.light }}";

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Light discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending light discovery message failed");
    }
}

void MyMqtt::sendMQTTLightModeDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/switch/curtains/light/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["ret"] = true;
    doc["name"] = "Light Mode";
    doc["cmd_t"] = lightModeTopic;
    doc["stat_t"] = lightModeTopic;

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Light mode discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending light mode discovery message failed");
    }
}

void MyMqtt::sendMQTTTimeModeDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/switch/curtains/time/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["ret"] = true;
    doc["name"] = "Time Mode";
    doc["cmd_t"] = timeModeTopic;
    doc["stat_t"] = timeModeTopic;

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Time mode discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending time mode discovery message failed");
    }
}

void MyMqtt::sendLightOpenTresholdDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/number/curtains/openTreshold/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["ret"] = true;
    doc["name"] = "Light open treshold";
    doc["cmd_t"] = openThresholdTopic;
    doc["stat_t"] = openThresholdTopic;
    doc["min"] = 0;
    doc["max"] = 500;
    doc["step"] = 20;
    doc["unit_of_meas"] = "lux";

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Light Open Treshold discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending Light Open Treshold discovery message failed");
    }
}

void MyMqtt::sendLightCloseTresholdDiscoveryMsg(PubSubClient &client)
{
    const char *discoveryTopic = "homeassistant/number/curtains/closeTreshold/config";

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["ret"] = true;
    doc["name"] = "Light close treshold";
    doc["cmd_t"] = closeThresholdTopic;
    doc["stat_t"] = closeThresholdTopic;
    doc["min"] = 0;
    doc["max"] = 500;
    doc["step"] = 20;
    doc["unit_of_meas"] = "lux";

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(discoveryTopic, buffer, n);
    if (publishedVerify)
    {
        Serial.println("Light Close Treshold discovery message send");
    }
    else
    {
        Serial.println("ERROR: Sending Light Close Treshold discovery message failed");
    }
}

void MyMqtt::sendInitialValues(PubSubClient &client, int openTreshold, int closeTreshold)
{
    String switchValue = "OFF";
    bool publishedVerify = client.publish(lightModeTopic.c_str(), switchValue.c_str(), switchValue.length());
    if (publishedVerify)
    {
        Serial.println("Light Mode default value published");
    }
    else
    {
        Serial.println("ERROR: Light Mode default value publishing failed");
    }
    publishedVerify = client.publish(timeModeTopic.c_str(), switchValue.c_str(), switchValue.length());
    if (publishedVerify)
    {
        Serial.println("Time Mode default value published");
    }
    else
    {
        Serial.println("ERROR: Time Mode default value publishing failed");
    }
    publishedVerify = client.publish(positionTopic.c_str(), String("50").c_str(), ((String) "50").length());
    if (publishedVerify)
    {
        Serial.println("Position default value published");
    }
    else
    {
        Serial.println("ERROR: Position default value publishing failed");
    }
    publishedVerify = client.publish(openThresholdTopic.c_str(), String(openTreshold).c_str(), String(openTreshold).length());
    if (publishedVerify)
    {
        Serial.println("Light open treshold default value published");
    }
    else
    {
        Serial.println("ERROR: Light open treshold default value publishing failed");
    }
    publishedVerify = client.publish(closeThresholdTopic.c_str(), String(closeTreshold).c_str(), String(closeTreshold).length());
    if (publishedVerify)
    {
        Serial.println("Light close treshold default value published");
    }
    else
    {
        Serial.println("ERROR: Light close treshold default value publishing failed");
    }
}

void MyMqtt::updateSensorValues(PubSubClient &client, int light, float temperature)
{
    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["light"] = light;
    doc["temperature"] = roundf(temperature * 10) / 10;

    size_t n = serializeJson(doc, buffer);

    bool publishedVerify = client.publish(sensorsStateTopic.c_str(), buffer, n);

    if (publishedVerify)
    {
        Serial.println("Sensors value succesfully updated");
    }
    else
    {
        Serial.println("ERROR: While updating sensors values");
    }
}

void MyMqtt::updatePosition(PubSubClient &client, int position)
{
    bool publishedVerify = client.publish(positionTopic.c_str(), String(position).c_str(), ((String)position).length());
    if (publishedVerify)
    {
        Serial.println("Position succesfully updated");
    }
    else
    {
        Serial.println("ERROR: While updating position");
    }
}
