#ifndef SENSORS_H
#define SENSORS_H
#include "Arduino.h"
#include "EmonLib.h"

namespace k_project{
    class Sensors{
        private:

        public:
            float intensity;
            float voltage;
            float power;

            //Measure interval:
            float interval;

            Sensors(/* args */);
            ~Sensors();

            void fakeData();

            double Current_Calculation();
            double Voltage_Calculation();
            double Power_Calculation();
        };
}

#endif