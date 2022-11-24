/*
    File: MyStepper.h
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Class and attributes declaration
*/

#ifndef MyStepper_h
#define MyStepper_h

#include <arduino.h>
#include <stdlib.h>
#include <string>
#include "math.h"
#include <MySchedule.h>
#include "MyThermistor.h"

class MyStepper
{
public:
    //
    // public functions
    // [1] - until line gap
    MyStepper();
    void connectToPins(byte stepPinNumber, byte directionPinNumber, byte reedSwitchPinNumber); // [1] - edited
    void enableStepper(void);                                                                  // [1]
    void disableStepper(void);                                                                 // [1]
    void setCurrentPositionInSteps(long currentPositionInSteps);                               // [1]
    long getCurrentPositionInSteps();                                                          // [1]
    void setSpeedInStepsPerSecond(float speedInStepsPerSecond);                                // [1]
    bool moveToHomeInSteps(long directionTowardHome, long maxDistanceToMoveInSteps);           // [1]
    void setupRelativeMoveInSteps(long distanceToMoveInSteps);                                 // [1] - edited
    void moveToPositionInSteps(long absolutePositionToMoveToInSteps);                          // [1]
    void setupMoveInSteps(long absolutePositionToMoveToInSteps);
    bool processMovement(void); // [1] - edited

    void moveFull(long dir);
    void setClosePosition(long current_position);
    void curtainsInit();
    void lightCheck(int lux);                                // Function handles motor if light mode is on
    void timeCheck(Week week, int hour, int min, char *day); // Function handles motor if time mode is on
    int getPositionInStepsFromPercent(int percent);
    int getPercentFromPositionInSteps(int position);

public:
    // [1] - until line gap
    byte stepPin;
    byte directionPin;
    byte reedSwitchPin;
    float desiredSpeed_InStepsPerSecond;
    float acceleration_InStepsPerSecondPerSecond;
    long targetPosition_InSteps;
    float stepsPerMillimeter;
    float stepsPerRevolution;
    bool startNewMove;
    float desiredStepPeriod_InUS;
    long decelerationDistance_InSteps;
    int direction_Scaler;
    float ramp_InitialStepPeriod_InUS;
    float ramp_NextStepPeriod_InUS;
    unsigned long ramp_LastStepTime_InUS;
    float acceleration_InStepsPerUSPerUS;
    float currentStepPeriod_InUS;
    long currentPosition_InSteps;
    long homePosition;
    long endPosition;

    long directionOpen;
    long directionClose;
    int speed;
    bool stopPointDetected;
    bool motorOverheated;
    int lightOpenThreshold;
    int lightCloseThreshold;
};

#endif

// [1] Speedy Stepper - https://github.com/Stan-Reifel/SpeedyStepper/blob/master
