#include "sensors.h"

namespace k_project{
    double Sensors::Current_Calculation(){
        EnergyMonitor Monitor;

        Monitor.current(34, 1.8); //ESP's PIN and calibration's valor (this valor is teoric)
    
        double current = Monitor.calcIrms(1484);

        return current;
    }

    double Sensors::Voltage_Calculation(){
        EnergyMonitor Monitor;

        Monitor.voltage(36, 0, 0); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

        double voltage = Monitor.readVcc(); //HABRÁ QUE CAMBIARLO, ES ORIENTATIVO

        return voltage;
    }

    double Sensors::Power_Calculation(){
        double power = Current_Calculation() * Voltage_Calculation();

        return power;
    }



    void Sensors::fakeData(){
        intensity = random(1,100)/random(1,10);
        voltage = random(1,100)/random(1,10);
        power = intensity * voltage;
    }
}