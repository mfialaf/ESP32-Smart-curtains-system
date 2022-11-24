/*
    File: MyMqtt.h
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Class and attributes declaration
*/

#ifndef MyMqtt_h
#define MyMqtt_h

#include <ArduinoJson.h>
#include <arduino.h>
#include <PubSubClient.h>

class MyMqtt
{
public:
    MyMqtt();

    // Discovery messages are for registering entities in Home assisstant
    void sendMQTTTemperatureDiscoveryMsg(PubSubClient &client);
    void sendMQTTLightDiscoveryMsg(PubSubClient &client);
    void sendMQTTCurtainsDiscoveryMsg(PubSubClient &client);
    void sendMQTTLightModeDiscoveryMsg(PubSubClient &client);
    void sendMQTTTimeModeDiscoveryMsg(PubSubClient &client);
    void sendMQTTUserModeDiscoveryMsg(PubSubClient &client);
    void sendLightOpenTresholdDiscoveryMsg(PubSubClient &client);
    void sendLightCloseTresholdDiscoveryMsg(PubSubClient &client);

    // Value updating methods
    void sendInitialValues(PubSubClient &client, int openTreshold, int closeTreshold);
    void updateSensorValues(PubSubClient &client, int light, float temperature);
    void updatePosition(PubSubClient &client, int position);

    String mqttName;
    String sensorsStateTopic;
    String positionTopic;
    String lightModeTopic;
    String timeModeTopic;
    String userModeTopic;
    String timetableTopic;
    String openThresholdTopic;
    String closeThresholdTopic;
    String logoutTopic;
};

#endif
