#include "sensors.h"

#define CALIBRATION_IRMS 1484
#define CALIBRATION_CURRENT 1.8
#define PIN_34 34
#define PIN_36 36

double Current_Calculation(){
    EnergyMonitor Monitor;

    Monitor.current(PIN_34, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)

    double current = Monitor.calcIrms(CALIBRATION_IRMS);

    return current;
}

double Voltage_Calculation(){
    EnergyMonitor Monitor;

    Monitor.voltage(PIN_36, 0, 0); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

    double voltage = Monitor.readVcc(); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

    return voltage;
}

double Power_Calculation(){
    double power = Current_Calculation() * Voltage_Calculation();

    return power;
}



void Sensors::fakeData(){
    intensity = random(1,100)/random(1,10);
    voltage = random(1,100)/random(1,10);
    power = intensity * voltage;
}