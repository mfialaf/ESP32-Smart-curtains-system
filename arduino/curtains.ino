/*
    File: curtains.ino
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Controling time, light and temperature sensors, communicating with MTQQ server.
                 Based on collected data evaluates operations with the stepper motor.
*/

#include <BH1750.h>
#include <MyStepper.h>
#include <MySchedule.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <MyThermistor.h>
#include <MyMqtt.h>
#include <stdio.h>
#include <ESP32Time.h>

// DEFINE PINS
#define EN_PIN 15
#define DIR_PIN 27
#define STEP_PIN 25
#define REED_SWITCH_PIN 19
#define THERMISTOR_PIN 34

// ---------------- CUSTOM PART ----------------
const char *WIFI_SSID = "XXX";  // HERE PLACE YOURS WIFI NAME
const char *WIFI_PASSWORD = "XXX"; // HERE PLACE YOURS WIFI PASSWORD

const char *mqttServer = "XXX.XXX.XXX.XXX"; // HERE PLACE THE IP OF YOURS MQTT BROKER
const int mqttPort = 1883;
const char *mqttUser = "YYY"; // HERE PLACE YOURS MQTT USERNAME
const char *mqttPassword = "YYY"; // HERE PLACE YOURS MQTT PASSWORD
// ---------------- CUSTOM PART ----------------

// Global variables keep actual system states
float temperature = 0;
int light = 0;
int positionInPercent = 50;
int previousPositionInPercent = 50;
long lastMsg = 0;
bool lightMode = false;
bool timeMode = false;

BH1750 lightMeter;
MyStepper mst;
Week week;
MyThermistor thermistor;
MyMqtt mqtt;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Process incoming messages from server to client
void callback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    // Changing possition of curtains
    if (String(topic) == mqtt.positionTopic)
    {
        int recievedPosition = messageTemp.toInt();
        // If the message is not duplicate or accidentaly reciever own message, process movement
        if (recievedPosition != positionInPercent && recievedPosition != previousPositionInPercent)
        {
            Serial.print("Changing output to ");
            Serial.println(messageTemp);
            int onCurtains = mst.getPositionInStepsFromPercent(recievedPosition);
            Serial.print("Position on curtains: ");
            Serial.println(onCurtains);
            mst.moveToPositionInSteps(onCurtains);
            if (mst.stopPointDetected)
            {
                Serial.println("ERROR: while moving, re-initializing curtains");
                mst.curtainsInit();
                mst.moveToPositionInSteps(onCurtains);
            }
            if (mst.motorOverheated)
            {
                Serial.println("ERROR: while moving, motor overheated waiting to cool down.");
                temperature = thermistor.getTemperature();
                mqtt.updateSensorValues(client, light, temperature);
                while (temperature > 50)
                {
                    Serial.print(".");
                    delay(15000);
                    temperature = thermistor.getTemperature();
                }
                mst.motorOverheated = false;
                mst.moveToPositionInSteps(onCurtains);
            }
        }
        delay(1000);
    }
    // Switching modes
    else if (String(topic) == mqtt.lightModeTopic)
    {
        if (messageTemp.compareTo("ON") == 0)
        {
            lightMode = true;
        }
        else
        {
            lightMode = false;
        }
    }
    else if (String(topic) == mqtt.timeModeTopic)
    {
        if (messageTemp.compareTo("ON") == 0)
        {
            timeMode = true;
        }
        else
        {
            timeMode = false;
        }
    }
    // Time mode timetable update
    else if (String(topic) == mqtt.timetableTopic)
    {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, messageTemp.c_str());
        week.processRequrst(doc);
        const char *day = doc["day"];
        week.printDayData(day);
    }
    // LightMode tresholds update
    else if (String(topic) == mqtt.openThresholdTopic)
    {
        mst.lightOpenThreshold = messageTemp.toInt();
        Serial.print("New light open treshold is [lux]: ");
        Serial.println(mst.lightOpenThreshold);
    }
    else if (String(topic) == mqtt.closeThresholdTopic)
    {
        mst.lightCloseThreshold = messageTemp.toInt();
        Serial.print("New light close treshold is [lux]: ");
        Serial.println(mst.lightCloseThreshold);
    }
    // Logout for debugging
    else if (String(topic) == mqtt.logoutTopic)
    {
        client.disconnect();
        Serial.print("Client disconected.");
        delay(10000);
    }
}

// Prcessed only once, at the begining when system starts
void setup()
{
    // Declare pin as input with pull up resistor
    pinMode(REED_SWITCH_PIN, INPUT_PULLUP);

    // Serial monitor
    Serial.begin(9600);
    Wire.begin();

    // Light sensor;
    lightMeter.begin();
    light = (int)lightMeter.readLightLevel();
    Serial.print("Light [lux]: ");
    Serial.println(light);

    // Thermistor
    thermistor.setThermistorPin(THERMISTOR_PIN);
    Serial.println("Temperature [°C]: ");
    temperature = thermistor.getTemperature();
    Serial.println(temperature);

    // Motor
    mst.connectToPins(STEP_PIN, DIR_PIN, REED_SWITCH_PIN);

    // Wifi
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to Wi-Fi");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    // MQTT
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    client.setKeepAlive(150); // Set longer keepAlive since moving curtains takes more that default 15 seconds
    Serial.println("Connecting to MQTT ");
    while (!client.connected())
    {
        Serial.print(".");

        if (client.connect(mqtt.mqttName.c_str(), mqttUser, mqttPassword))
        {
            // Subscription for topics coming from broker
            Serial.println("Connected to MQTT");
            client.subscribe(mqtt.positionTopic.c_str());
            client.subscribe(mqtt.lightModeTopic.c_str());
            client.subscribe(mqtt.timeModeTopic.c_str());
            client.subscribe(mqtt.timetableTopic.c_str());
            client.subscribe(mqtt.openThresholdTopic.c_str());
            client.subscribe(mqtt.closeThresholdTopic.c_str());
            client.subscribe(mqtt.logoutTopic.c_str());

            // Register entities in home assistant
            mqtt.sendMQTTCurtainsDiscoveryMsg(client);
            mqtt.sendMQTTLightDiscoveryMsg(client);
            mqtt.sendMQTTTemperatureDiscoveryMsg(client);
            mqtt.sendMQTTLightModeDiscoveryMsg(client);
            mqtt.sendMQTTTimeModeDiscoveryMsg(client);
            mqtt.sendLightOpenTresholdDiscoveryMsg(client);
            mqtt.sendLightCloseTresholdDiscoveryMsg(client);

            // Init values, to match with defaulf values in code
            mqtt.sendInitialValues(client, mst.lightOpenThreshold, mst.lightCloseThreshold);
            // Send first ligt and temperature data
            mqtt.updateSensorValues(client, light, temperature);
            client.loop();
        }
        else
        {
            Serial.println("MQTT failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Time
    ESP32Time.begin(); //< adjusted the time

    // Init curtains
    mst.curtainsInit();
    Serial.print("home [steps]: ");
    Serial.println(mst.homePosition);
    Serial.print("end [steps]: ");
    Serial.println(mst.endPosition);
    // After init send set position to 50% of curtains lenght
    long a = mst.getPositionInStepsFromPercent(50);
    mst.moveToPositionInSteps(a);
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT re - connection...");
        // Attempt to connect
        if (client.connect(mqtt.mqttName.c_str(), mqttUser, mqttPassword))
        {
            Serial.println("connected to MQTT client");
            client.loop();
            // Subscribe
            client.subscribe(mqtt.positionTopic.c_str());
            client.subscribe(mqtt.lightModeTopic.c_str());
            client.subscribe(mqtt.timeModeTopic.c_str());
            client.subscribe(mqtt.userModeTopic.c_str());
            client.subscribe(mqtt.openThresholdTopic.c_str());
            client.subscribe(mqtt.closeThresholdTopic.c_str());
            client.subscribe(mqtt.logoutTopic.c_str());
            break;
        }
        else
        {
            Serial.print("ERROR: failed with state: ");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop()
{
    long now = millis();

    if (WiFi.status() == WL_CONNECTED)
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();

        // It's not neccessary to read and update data every time loop iterates,
        // timer is set to 15 sec.
        if (now - lastMsg > 15000)
        {
            lastMsg = now;

            // Read light sensor
            light = (int)lightMeter.readLightLevel();
            Serial.print("Light [lux]: ");
            Serial.println(light);
            if (lightMode)
            {
                // Check if because of light should move the curtains
                mst.lightCheck((int)light);
            }
            delay(25); // Wait a second

            // Thermistor
            Serial.println("Temperature [°C]: ");
            temperature = thermistor.getTemperature();
            Serial.println(temperature);

            // Get current time
            time_t t = time(NULL);
            struct tm *t_st;
            t_st = localtime(&t);
            int hour = t_st->tm_hour;
            int min = t_st->tm_min;
            // Serial.print("hour: ");
            // Serial.println(hour);
            // Serial.print("minute: ");
            // Serial.println(min);
            char day[] = "XXX";
            strncpy(day, ctime(&t), 3); // Set the day name from structure
            // Serial.println(day);
            if (timeMode)
            {
                // Check if it's time to move the curtains
                mst.timeCheck(week, hour, min, day);
            }
            delay(25); // Wait a second

            // After loading new values, update them on server as well
            mqtt.updateSensorValues(client, light, temperature);
        }

        long currentPos = mst.getCurrentPositionInSteps();
        int currentPosInPercent = mst.getPercentFromPositionInSteps(currentPos);
        // If curtains were moved, update values on server
        if (currentPosInPercent != positionInPercent)
        {
            previousPositionInPercent = positionInPercent;
            positionInPercent = currentPosInPercent;
            // Publish position
            mqtt.updatePosition(client, positionInPercent);
        }
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}
