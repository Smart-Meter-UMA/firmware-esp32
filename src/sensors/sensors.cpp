#include "sensors.h"

EnergyMonitor Monitor;


Sensors::Sensors(){
    Monitor.current(PIN_CURRENT, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)
    Monitor.voltage(PIN_VOLTAGE, CALIBRATION_VOLTAGE, CALIBRATION_PHASE); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

    current = 0.0;
    voltage = 0.0;
    power = 0.0;
}

double Sensors::currentCalculation(){
    current = abs(Monitor.calcIrms(CALIBRATION_IRMS) - 0.6);

    return current;
}

double Sensors::voltageCalculation(){
    #ifdef USE_FIXED_VOLTAGE
    voltage = FIXED_VOLTAGE;
    #endif

    #ifndef USE_FIXED_VOLTAGE
    voltage = Monitor.readVcc(); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

    #endif

    //voltage = abs(Monitor.calcVrms(CALIBRATION_VRMS));
    //voltage = analogRead(34) *(3.3 / 4096);

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