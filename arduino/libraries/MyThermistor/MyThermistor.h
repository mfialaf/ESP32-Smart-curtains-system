/*
    File: MyThermistor.h
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Class and attributes declaration
*/

#ifndef MyThermistor_h
#define MyThermistor_h

#include <arduino.h>
#include <stdlib.h>
#include <math.h>

class MyThermistor
{
private:
    byte thermistorPin;
    int nominalTemperature;
    int nominalResistance;
    int BCoeficient;
    int resistorResistance;

public:
    MyThermistor();
    void setThermistorPin(byte thermistorPinNumber);
    float getTemperature();
};

#endif
