/*
    File: MySchedule.h
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Class and attributes declaration
*/

#ifndef MySchedule_h
#define MySchedule_h

#include <arduino.h>
#include <iostream>
#include <list>
#include <string>
#include <ArduinoJson.h>

using namespace std;

#define OPEN 0
#define CLOSE 100

// Storing one record - exact time with position
class TimeAndPosition
{
private:
    int hours;
    int mins;
    int position;

public:
    TimeAndPosition();
    TimeAndPosition(int hoursToSet, int minsToSet, int posToSet);
    void setHours(int hoursToSet);
    void setMins(int minsToSet);
    void setPosition(int posToSet);
    int getHours();
    int getMins();
    int getPosition();
    void vypis();
};

// Represents weekdays, which can be filled with TimeAndPosition records
class Week{
    public:
    list<TimeAndPosition> monday;
    list<TimeAndPosition> tuesday;
    list<TimeAndPosition> wednesday;
    list<TimeAndPosition> thursday;
    list<TimeAndPosition> friday;
    list<TimeAndPosition> saturday;
    list<TimeAndPosition> sunday;
    
    Week();
    void printDayData(const char *day);
    void processRequrst(DynamicJsonDocument doc);
    int getPosition(char *day, int hours, int mins);
};

#endif
