/*
    File: MyStepper.cpp
    Author: Marek Fiala
    Project: INTELLIGENT WINDOW SHUTTER SYSTEM
    Date: 07.05.2022
    Description: Controling stepper motor movement
*/

#include "MyStepper.h"

MyStepper::MyStepper()
{
    //
    // initialize constants
    //
    stepPin = 0;
    directionPin = 0;
    reedSwitchPin = 0;
    stepsPerRevolution = 1600.0;
    stepsPerMillimeter = 40.0;
    currentPosition_InSteps = 0;
    desiredSpeed_InStepsPerSecond = 750.0;
    acceleration_InStepsPerSecondPerSecond = 250.0;
    directionOpen = -1;
    directionClose = 1;
    stopPointDetected = false;
    motorOverheated = false;
    lightOpenThreshold = 10;
    lightCloseThreshold = 160;
    //   currentStepPeriod_InUS = 0.0;
}

//
// connect the stepper object to the IO pins
//  Enter:  stepPinNumber = IO pin number for the Step
//          directionPinNumber = IO pin number for the direction bit
//          reedSwitchPinNumber = IO pin number for the stopper
//
//
void MyStepper::connectToPins(byte stepPinNumber, byte directionPinNumber, byte reedSwitchPinNumber)
{
    //
    // remember the pin numbers
    //
    stepPin = stepPinNumber;
    directionPin = directionPinNumber;
    reedSwitchPin = reedSwitchPinNumber;

    //
    // configure the IO bits
    //
    pinMode(stepPin, OUTPUT);
    digitalWrite(stepPin, LOW);

    pinMode(directionPin, OUTPUT);
    digitalWrite(directionPin, LOW);

    pinMode(reedSwitchPin, INPUT_PULLUP);
}

//
// setup a move relative to the current position, units are in steps, no motion
// occurs until processMove() is called.  Note: this can only be called when the
// motor is stopped
//  Enter:  distanceToMoveInSteps = signed distance to move relative to the current
//          position in steps
//
void MyStepper::setupRelativeMoveInSteps(long distanceToMoveInSteps)
{
    setupMoveInSteps(currentPosition_InSteps + distanceToMoveInSteps);
}

//
// set the maximum speed, units in steps/second, this is the maximum speed reached
// while accelerating
// Note: this can only be called when the motor is stopped
//  Enter:  speedInStepsPerSecond = speed to accelerate up to, units in steps/second
//
void MyStepper::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
}

//
// set the current position of the motor in steps, this does not move the motor
// Note: This function should only be called when the motor is stopped
//    Enter:  currentPositionInSteps = the new position of the motor in steps
//
void MyStepper::setCurrentPositionInSteps(long currentPositionInSteps)
{
    currentPosition_InSteps = currentPositionInSteps;
}

//
// get the current position of the motor in steps, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in steps returned
//
long MyStepper::getCurrentPositionInSteps()
{
    return (currentPosition_InSteps);
}

//
// move to the given absolute position, units are in steps, this function does not
// return until the move is complete
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to
//            in units of steps
//
void MyStepper::moveToPositionInSteps(long absolutePositionToMoveToInSteps)
{
    setupMoveInSteps(absolutePositionToMoveToInSteps);
    Serial.print("Moving to [steps]: ");
    Serial.println(absolutePositionToMoveToInSteps);
    while (!processMovement())
        ;
}

bool MyStepper::moveToHomeInSteps(long directionTowardHome, long maxDistanceToMoveInSteps)
{
    bool limitSwitchFlag;

    //
    // if the home switch is not already set, move toward it
    //
    if (digitalRead(reedSwitchPin) == HIGH)
    {
        // Serial.println("2");

        //
        // move toward the home switch
        //
        setupRelativeMoveInSteps(maxDistanceToMoveInSteps * directionTowardHome);
        limitSwitchFlag = false;
        while (!processMovement())
        {
            if (digitalRead(reedSwitchPin) == LOW)
            {
                limitSwitchFlag = true;
                break;
            }
        }

        //
        // check if switch never detected
        //
        if (limitSwitchFlag == false)
            return (false);
    }
    delay(25);

    //
    // the switch has been detected, now move away from the switch
    //
    setupRelativeMoveInSteps(maxDistanceToMoveInSteps * directionTowardHome * -1);
    limitSwitchFlag = false;

    while (!processMovement())
    {
        if (digitalRead(reedSwitchPin) == HIGH)
        {
            limitSwitchFlag = true;
            break;
        }
    }
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false)
        return (false);

    //
    // Home or end position should be a litle further than the REEDSWITCH detected
    //
    setupRelativeMoveInSteps(400 * directionTowardHome * -1);
    while (!processMovement())
        ;
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false)
        return (false);

    //
    // successfully homed, set the current position to 0 or end
    //
    if (directionTowardHome == directionOpen)
    {
        setCurrentPositionInSteps(0L);
        homePosition = currentPosition_InSteps;
    }
    else
    {
        endPosition = currentPosition_InSteps;
    }

    return (true);
}

//
// if it is time, move one step
//  Exit:  true returned if movement complete, false returned not a final target
//           position yet
//
bool MyStepper::processMovement(void)
{

    unsigned long currentTime_InUS;
    unsigned long periodSinceLastStep_InUS;
    long distanceToTarget_InSteps;

    //
    // check if already at the target position
    //
    if (currentPosition_InSteps == targetPosition_InSteps)
    {
        return (true);
    }

    // Motor overheated
    // if (thermistor_stepper.getTemperature() > 50)
    // {
    //     Serial.println("ERROR: motor overheated");
    //     motorOverheated = true;
    //     return (true);
    // }


    // Error: arrived to stop point
    if (digitalRead(reedSwitchPin) == LOW && stopPointDetected == false)
    {
        Serial.println("ERROR: curtains behind stopper");
        stopPointDetected = true;
        return (true);
    }

    //
    // check if this is the first call to start this new move
    //
    if (startNewMove)
    {
        ramp_LastStepTime_InUS = micros();
        startNewMove = false;
    }

    //
    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    //
    currentTime_InUS = micros();
    periodSinceLastStep_InUS = currentTime_InUS - ramp_LastStepTime_InUS;

    //
    // if it is not time for the next step, return
    //
    if (periodSinceLastStep_InUS < (unsigned long)ramp_NextStepPeriod_InUS)
        return (false);

    //
    // determine the distance from the current position to the target
    //
    distanceToTarget_InSteps = targetPosition_InSteps - currentPosition_InSteps;
    if (distanceToTarget_InSteps < 0)
        distanceToTarget_InSteps = -distanceToTarget_InSteps;

    //
    // test if it is time to start decelerating, if so change from accelerating to
    // decelerating
    //
    if (distanceToTarget_InSteps == decelerationDistance_InSteps)
        acceleration_InStepsPerUSPerUS = -acceleration_InStepsPerUSPerUS;

    //
    // execute the step on the rising edge
    //
    digitalWrite(stepPin, HIGH);

    //
    // delay set to almost nothing because there is so much code between rising and
    // falling edges
    delayMicroseconds(2);

    //
    // update the current position and speed
    //
    currentPosition_InSteps += direction_Scaler;
    currentStepPeriod_InUS = ramp_NextStepPeriod_InUS;

    //
    // compute the period for the next step
    // StepPeriodInUS = LastStepPeriodInUS *
    //   (1 - AccelerationInStepsPerUSPerUS * LastStepPeriodInUS^2)
    //
    ramp_NextStepPeriod_InUS = ramp_NextStepPeriod_InUS *
                               (1.0 - acceleration_InStepsPerUSPerUS * ramp_NextStepPeriod_InUS *
                                          ramp_NextStepPeriod_InUS);

    //
    // return the step line high
    //
    digitalWrite(stepPin, LOW);

    //
    // clip the speed so that it does not accelerate beyond the desired velocity
    //
    if (ramp_NextStepPeriod_InUS < desiredStepPeriod_InUS)
        ramp_NextStepPeriod_InUS = desiredStepPeriod_InUS;

    //
    // update the acceleration ramp
    //
    ramp_LastStepTime_InUS = currentTime_InUS;

    //
    // check if move has reached its final target position, return true if all done
    //
    if (currentPosition_InSteps == targetPosition_InSteps)
    {
        currentStepPeriod_InUS = 0.0;
        return (true);
    }

    return (false);
}

//
// setup a move, units are in steps, no motion occurs until processMove() is called
// Note: this can only be called when the motor is stopped
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to in
//          units of steps
//
void MyStepper::setupMoveInSteps(long absolutePositionToMoveToInSteps)
{
    long distanceToTravel_InSteps;

    //
    // save the target location
    //
    targetPosition_InSteps = absolutePositionToMoveToInSteps;

    //
    // determine the period in US of the first step
    //
    ramp_InitialStepPeriod_InUS = 1000000.0 / sqrt(2.0 *
                                                   acceleration_InStepsPerSecondPerSecond);

    //
    // determine the period in US between steps when going at the desired velocity
    //
    desiredStepPeriod_InUS = 1000000.0 / desiredSpeed_InStepsPerSecond;

    //
    // determine the number of steps needed to go from the desired velocity down to a
    // velocity of 0, Steps = Velocity^2 / (2 * Accelleration)
    //
    decelerationDistance_InSteps = (long)round((desiredSpeed_InStepsPerSecond *
                                                desiredSpeed_InStepsPerSecond) /
                                               (2.0 * acceleration_InStepsPerSecondPerSecond));

    //
    // determine the distance and direction to travel
    //
    distanceToTravel_InSteps = targetPosition_InSteps - currentPosition_InSteps;
    if (distanceToTravel_InSteps < 0)
    {
        distanceToTravel_InSteps = -distanceToTravel_InSteps;
        direction_Scaler = -1;
        digitalWrite(directionPin, HIGH);
    }
    else
    {
        direction_Scaler = 1;
        digitalWrite(directionPin, LOW);
    }

    //
    // check if travel distance is too short to accelerate up to the desired velocity
    //
    if (distanceToTravel_InSteps <= (decelerationDistance_InSteps * 2L))
        decelerationDistance_InSteps = (distanceToTravel_InSteps / 2L);

    //
    // start the acceleration ramp at the beginning
    //
    ramp_NextStepPeriod_InUS = ramp_InitialStepPeriod_InUS;
    acceleration_InStepsPerUSPerUS = acceleration_InStepsPerSecondPerSecond / 1E12;
    startNewMove = true;
}

void MyStepper::moveFull(long dir)
{

    const long maxHomingDistanceInMM = 2147483647; // max hodnota long == cca 53 metru

    if (moveToHomeInSteps(dir, maxHomingDistanceInMM) != true)
    {
        if (dir == directionOpen)
            Serial.println("ERROR: while opening");
        else
            Serial.println("ERROR: while closing");
    }
    else
    {
        if (dir == directionOpen)
            Serial.println("Succesfully opened");
        else
            Serial.println("Succesfully closed");
    }
}



void MyStepper::setClosePosition(long current_position)
{
    endPosition = current_position;
}

void MyStepper::curtainsInit()
{
    Serial.println("Curtains initialization ");
    stopPointDetected = true; // When initializing, REED_SWITCH is not used as stopPoint but end to end measurement
    moveFull(directionOpen);
    delay(500);
    moveFull(directionClose);
    stopPointDetected = false;
    // Serial.print("Curtains initialization: OPENED");

    Serial.print("Curtains length [mm]");
    Serial.println((long)round(endPosition / stepsPerMillimeter));
    delay(1000);
}

void MyStepper::lightCheck(int lux)
{

    // Curtains closed, and sensor says its time to open
    if (getCurrentPositionInSteps() > (0.05 * endPosition) && lux <= lightOpenThreshold)
    {
        Serial.println("Detected light value is less than treshold for open. OPENING");

        moveToPositionInSteps(homePosition);
        if (stopPointDetected)
        {
            Serial.println("ERROR: While moving, re-initializing curtains");
            curtainsInit();
            lightCheck(lux);
        }
    }

    // Curtains open, and sensor says its time to close
    if (getCurrentPositionInSteps() < (0.95 * endPosition) && lux >= lightCloseThreshold)
    {
        Serial.println("Detected light value is more than treshold for close. CLOSING");
        moveToPositionInSteps(endPosition);
        if (stopPointDetected)
        {
            Serial.println("ERROR: While moving, re-initializing curtains");
            curtainsInit();
            lightCheck(lux);
        }
    }
}

void MyStepper::timeCheck(Week week, int hour, int min, char *day)
{
    int position = week.getPosition(day, hour, min);
    if (position != -1)
    {
        Serial.print("It's ");
        Serial.print(day);
        Serial.print(", ");
        Serial.print(hour);
        Serial.print(":");
        Serial.print(min);
        Serial.print(", movin to position [%]: ");
        Serial.println(position);
        int onCurtains = getPositionInStepsFromPercent(position);
        // Serial.print("Position on curtains: ");
        // Serial.println(onCurtains);
        moveToPositionInSteps(onCurtains);
        if (stopPointDetected)
        {
            Serial.println("ERROR: While moving, re-initializing curtains");
            curtainsInit();
            timeCheck(week, hour, min, day);
        }
    }
}

int MyStepper::getPositionInStepsFromPercent(int percent)
{
    float pos = (float)endPosition * percent / 100;
    // Serial.print("Pozice je: ");
    // Serial.println(pos);
    int zaokrouhleni = roundf(pos);
    // Serial.print("Zaokrouhleno na: ");
    // Serial.println(zaokrouhleni);
    return zaokrouhleni;
    // return roundf(endPosition * percent / 100);
}

int MyStepper::getPercentFromPositionInSteps(int position)
{
    float perc = 100 * position / (float)endPosition;
    // Serial.print("Procento je: ");
    // Serial.println(perc);
    int zaokrouhleni = roundf(perc);
    // Serial.print("Zaokrouhleno na: ");
    // Serial.println(zaokrouhleni);
    return zaokrouhleni;
    // return roundf(100 * position / endPosition);
}
