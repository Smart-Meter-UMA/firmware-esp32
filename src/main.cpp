#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ble/ble.h>
#include <sensors/sensors.h>
#include <wifi/wifi.h>

#define CONFIG_NIMBLE_CPP_DEBUG_LEVEL = 4

uint32_t current_time;
uint32_t start_time;
uint32_t timed_event;

uint32_t interval;

Sensors sensors;

EnergyMonitor Monitor;

void changeValueOfRegisters(){
  float randomNumber = random(1,100)/random(1,10);
}

void setup() {
  Serial.begin(9600);

  setupBLE();

  timed_event = 1000; // after 1000 ms trigger the event
  current_time = millis();
  start_time = current_time;

  Monitor.current(PIN_34, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)

  sensors = Sensors();
}

void loop() {
  current_time = millis(); // update the timer every cycle

	if (current_time - start_time >= timed_event) {
		// the event to trigger

		setValueCharacteristic("voltage", std::to_string(sensors.voltageCalculation()));
		setValueCharacteristic("current", std::to_string(sensors.currentCalculation()));
		setValueCharacteristic("power", std::to_string(sensors.powerCalculation()));

		Serial.println("FUNCIONES: ");
		Serial.print("Voltage: ");
		Serial.println(sensors.voltageCalculation());
		Serial.print("Current: ");
		Serial.println(sensors.currentCalculation());
		Serial.print("Power: "); 
		Serial.println(sensors.powerCalculation());
		Serial.println();
		Serial.println();
		
		//TODO: Need to check if the value is correct:
		interval = std::stoi(getValueCharacteristic("interval"));
		timed_event = interval * 1000;
		start_time = current_time;  // reset the timer
	}
}