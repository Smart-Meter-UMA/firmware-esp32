#ifndef SENSORS_H
#define SENSORS_H
#include "Arduino.h"
#include "EmonLib.h"

#define USE_FIXED_VOLTAGE 1
#define FIXED_VOLTAGE 240

#define CALIBRATION_IRMS 1484
#define CALIBRATION_CURRENT 111.1
#define CALIBRATION_VRMS 2000
#define CALIBRATION_VOLTAGE 175
#define CALIBRATION_PHASE 1.7
#define PIN_CURRENT 34
#define PIN_VOLTAGE 35

class Sensors{
    private:

    public:
        double current;
        double voltage;
        double power;

        //Measure interval:
        float interval;

        EnergyMonitor Monitor;

        Sensors(/* args */);

        void fakeData();

        double currentCalculation();
        double voltageCalculation();
        double powerCalculation();
    };

#endif