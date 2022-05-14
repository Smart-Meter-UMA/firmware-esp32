#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ble/ble.h>
#include <sensors/sensors.cpp>

uint32_t current_time;
uint32_t start_time;
uint32_t timed_event;

Sensors sensors;

void changeValueOfRegisters(){
  float randomNumber = random(1,100)/random(1,10);

}

void setup() {
  Serial.begin(9600);

  setupBLE();

  timed_event = 1000; // after 1000 ms trigger the event
  current_time = millis();
  start_time = current_time;

  sensors = Sensors();
}

void loop() {
  current_time = millis(); // update the timer every cycle

	if (current_time - start_time >= timed_event) {
		// the event to trigger
		Serial.println("."); 
		
		sensors.fakeData();
		setValueCharacteristic("voltage", std::to_string(sensors.voltage));
		setValueCharacteristic("intensity", std::to_string(sensors.voltage));
		setValueCharacteristic("power", std::to_string(sensors.voltage));
		start_time = current_time;  // reset the timer
	}
}