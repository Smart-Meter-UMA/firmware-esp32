#include <Arduino.h>

class Sensors
{
private:
public:
    float intensity;
    float voltage;
    float power;

    //Measure interval:
    float interval;

    Sensors(/* args */){

    }
    ~Sensors(){

    }

    void fakeData(){
        intensity = random(1,100)/random(1,10);
        voltage = random(1,100)/random(1,10);
        power = intensity * voltage;
    }
};