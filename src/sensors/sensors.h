#ifndef SENSORS_H
#define SENSORS_H
#include "Arduino.h"
#include "EmonLib.h"

#define USE_FIXED_VOLTAGE 1
#define FIXED_VOLTAGE 240

#define CALIBRATION_IRMS 1484
#define CALIBRATION_CURRENT 1.8
#define PIN_34 34
#define PIN_36 36

class Sensors{
    private:

    public:
        double current;
        double voltage;
        double power;

        //Measure interval:
        float interval;

        Sensors(/* args */);

        void fakeData();

        double currentCalculation();
        double voltageCalculation();
        double powerCalculation();
    };

#endif