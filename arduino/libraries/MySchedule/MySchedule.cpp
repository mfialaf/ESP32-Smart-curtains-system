/*
    File: MySchedule.cpp
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Schedule with week days storing possition at exact time
*/

#include "MySchedule.h"

TimeAndPosition::TimeAndPosition()
{
    hours = 0;
    mins = 0;
    position = 0; // [%]
}

TimeAndPosition::TimeAndPosition(int hoursToSet, int minsToSet, int posToSet)
{
    setHours(hoursToSet);
    setMins(minsToSet);
    setPosition(posToSet);
}

void TimeAndPosition::setHours(int hoursToSet)
{
    if (hoursToSet > 23)
        hours = 0;
    else
        hours = hoursToSet;
}

void TimeAndPosition::setMins(int minsToSet)
{
    if (minsToSet > 60)
        mins = 0;
    else
        mins = minsToSet;
}

void TimeAndPosition::setPosition(int posToSet)
{
    if (posToSet > 100)
        position = 0;
    else
        position = posToSet;
}

int TimeAndPosition::getHours()
{
    return hours;
}

int TimeAndPosition::getMins()
{
    return mins;
}

int TimeAndPosition::getPosition()
{
    return position;
}

void TimeAndPosition::vypis()
{
    //   cout << "hours: " << getHours() << endl;
    //   cout << "mins: " << getMins() << endl;
    //   cout << "position: " << getPosition() << endl;
    Serial.print("  ");
    Serial.print(getHours());
    if (String(getHours()).length() == 1)
    {
        Serial.print("     |   ");
    }
    else
    {
        Serial.print("    |   ");
    }
    Serial.print(getMins());
    if (String(getMins()).length() == 1)
    {
        Serial.print("     |     ");
    }
    else
    {
        Serial.print("    |     ");
    }
    Serial.println(getPosition());
}

Week::Week()
{
    list<TimeAndPosition>::iterator it;
    it = monday.begin();
    TimeAndPosition a(10, 10, CLOSE);
    monday.insert(it, TimeAndPosition(10, 10, CLOSE));
    TimeAndPosition b(20, 20, OPEN);
    monday.insert(it, b);
    tuesday = {TimeAndPosition(11, 31, CLOSE), TimeAndPosition(17, 41, OPEN)};
    wednesday = {TimeAndPosition(16, 36, 20), TimeAndPosition(16, 37, CLOSE), TimeAndPosition(16, 38, 60), TimeAndPosition(16, 40, 25)};
    thursday = {TimeAndPosition(13, 33, CLOSE), TimeAndPosition(19, 43, OPEN)};
    friday = {TimeAndPosition(14, 34, CLOSE), TimeAndPosition(18, 44, OPEN)};
    saturday = {TimeAndPosition(15, 35, CLOSE), TimeAndPosition(19, 45, OPEN)};
    sunday = {TimeAndPosition(13, 53, CLOSE), TimeAndPosition(13, 54, 75)};
}

void Week::printDayData(const char *day)
{
    if (string(day).compare("Mon") == 0)
    {
        Serial.println("--- --- --- MONDAY --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = monday.begin(); it != monday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Tue") == 0)
    {
        Serial.println("--- --- --- Tuesday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = tuesday.begin(); it != tuesday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Wed") == 0)
    {
        Serial.println("--- --- --- Wednesday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = wednesday.begin(); it != wednesday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Thu") == 0)
    {
        Serial.println("--- --- --- Thursday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = thursday.begin(); it != thursday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Fri") == 0)
    {
        Serial.println("--- --- --- Friday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = friday.begin(); it != friday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Sat") == 0)
    {
        Serial.println("--- --- --- Saturday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = saturday.begin(); it != saturday.end(); ++it)
        {
            (*it).vypis();
        }
    }
    else if (string(day).compare("Sun") == 0)
    {
        Serial.println("--- --- --- Sunday --- --- ---");
        Serial.println(" hour   |   min   |   position");
        for (auto it = sunday.begin(); it != sunday.end(); ++it)
        {
            (*it).vypis();
        }
    }
}

// add or remove record from week
// attr doc is JSON formated message
void Week::processRequrst(DynamicJsonDocument doc)
{
    const char *action = doc["action"]; // add , remove
    const char *day = doc["day"];       // week days example - Mon
    int hours = doc["hour"];
    int mins = doc["min"];
    int position = doc["position"];

    if (string(day).compare("Mon") == 0)
    {
        bool found = false;
        for (auto it = monday.begin(); it != monday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = monday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            monday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Tue") == 0)
    {
        bool found = false;
        for (auto it = tuesday.begin(); it != tuesday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = tuesday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            tuesday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Wed") == 0)
    {
        bool found = false;
        for (auto it = wednesday.begin(); it != wednesday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = wednesday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            wednesday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Thu") == 0)
    {
        bool found = false;
        for (auto it = thursday.begin(); it != thursday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = thursday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            thursday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Fri") == 0)
    {
        bool found = false;
        for (auto it = friday.begin(); it != friday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = friday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            friday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Sat") == 0)
    {
        bool found = false;
        for (auto it = saturday.begin(); it != saturday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = saturday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            saturday.push_back(TimeAndPosition(hours, mins, position));
    }
    else if (string(day).compare("Sun") == 0)
    {
        bool found = false;
        for (auto it = sunday.begin(); it != sunday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
            {
                found = true;
                if (String("add").compareTo(action) == 0)
                {
                    it->setPosition(position);
                }
                else if (String("remove").compareTo(action) == 0)
                {
                    it = sunday.erase(it);
                }
            }
        }
        if (!found && String("add").compareTo(action) == 0)
            sunday.push_back(TimeAndPosition(hours, mins, position));
    }
}

// Returns position if found, otherwise -1
int Week::getPosition(char *day, int hours, int mins)
{
    if (string(day).compare("Mon") == 0)
    {
        for (auto it = monday.begin(); it != monday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Tue") == 0)
    {
        for (auto it = tuesday.begin(); it != tuesday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Wed") == 0)
    {
        for (auto it = wednesday.begin(); it != wednesday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Thu") == 0)
    {
        for (auto it = thursday.begin(); it != thursday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Fri") == 0)
    {
        for (auto it = friday.begin(); it != friday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Sat") == 0)
    {
        for (auto it = saturday.begin(); it != saturday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    else if (string(day).compare("Sun") == 0)
    {
        for (auto it = sunday.begin(); it != sunday.end(); ++it)
        {
            if ((*it).getHours() == hours && mins == it->getMins())
                return it->getPosition();
        }
    }
    return -1;
}
