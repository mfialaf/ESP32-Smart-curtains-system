/*
    File: MyThermistor.cpp
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Getting data from thermistor and converting the value to degrees celstius
*/

#include "MyThermistor.h"

MyThermistor::MyThermistor()
{
    thermistorPin = 0;
    nominalTemperature = 25;
    nominalResistance = 10000;
    BCoeficient = 12920;
    resistorResistance = 10000;
}

void MyThermistor::setThermistorPin(byte thermistorPinNumber)
{
    thermistorPin = thermistorPinNumber;
}

// [1]
float MyThermistor::getTemperature()
{
    uint8_t i;
    float average;
    int samplesCount = 5;
    int samples[samplesCount];

    // take N samples in a row, with a slight delay
    for (i = 0; i < samplesCount; i++)
    {
        samples[i] = analogRead(thermistorPin);
        delay(10);
    }

    // average all the samples out
    average = 0;
    for (i = 0; i < samplesCount; i++)
    {
        average += samples[i];
    }
    average /= samplesCount;

    // Serial.print("Average analog reading ");
    // Serial.println(average);

    // convert the value to resistance

    // For U1
    // average = 4095 / average - 1;
    // average = resistorResistance / average;

    // For U2
    average = (4095 - average) / average;
    average = resistorResistance * average;
    // Serial.print("Thermistor resistance ");
    // Serial.println(average);

    float steinhart;
    steinhart = average / nominalResistance;          // (R/Ro)
    steinhart = log(steinhart);                       // ln(R/Ro)
    steinhart /= BCoeficient;                         // 1/B * ln(R/Ro)
    steinhart += 1.0 / (nominalTemperature + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // convert absolute temp to C

    // Serial.print("Temperature ");
    // Serial.print(steinhart);
    // Serial.println(" *C");

    return steinhart;
}

// [1] https://github.com/adafruit/Adafruit_Learning_System_Guides/blob/main/Themistor/Example3/Example3.ino
