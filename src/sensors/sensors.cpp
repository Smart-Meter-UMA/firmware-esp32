#include "sensors.h"

Sensors::Sensors(){
    Monitor.current(PIN_34, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)
    //Monitor.voltage(PIN_36, 0, 0); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO
}

double Sensors::currentCalculation(){
    current = abs(Monitor.calcIrms(CALIBRATION_IRMS) - 0.04);

    return current;
}

double Sensors::voltageCalculation(){
    #ifdef USE_FIXED_VOLTAGE
    voltage = FIXED_VOLTAGE;
    #endif

    #ifndef USE_FIXED_VOLTAGE
    voltage = Monitor.readVcc(); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

    #endif

    return voltage;
}

double Sensors::powerCalculation(){
    power = currentCalculation() * voltageCalculation();

    return power;
}

void Sensors::fakeData(){
    current = random(1,100)/random(1,10);
    voltage = random(1,100)/random(1,10);
    power = current * voltage;
}