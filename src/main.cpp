#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ble/ble.h>
#include <sensors/sensors.h>
#include <wifi/wifimanager.h>

#define CONFIG_NIMBLE_CPP_DEBUG_LEVEL 4
#define ESP32 1

//DEEP-SLEEP
//#define uS_TO_S 1000000 //Conversion factor for micro seconds to seconds
//#define TIME_TO_SLEEP 5 //Time ESP32 will go to sleep (in seconds)

//LIGHT-SLEEP
#define uS_TO_S 1000000ULL //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 5 //Time ESP32 will go to sleep (in seconds)


uint32_t current_time;
uint32_t start_time;
uint32_t timed_event;

uint32_t interval;

Sensors sensors;

EnergyMonitor Monitor;

WifiManager wifiManager;

CharacteristicWifiCallbacks *wifiCallbacks;

void changeValueOfRegisters(){
  float randomNumber = random(1, 100) / random(1, 10);
}


//DEEP-SLEEP & LIGHT-SLEEP 				IT ISN'T NECESARY
void printWakeUpReason(){
	esp_sleep_wakeup_cause_t wakeUpReason;
	wakeUpReason = esp_sleep_get_wakeup_cause();

	switch(wakeUpReason){
		case ESP_SLEEP_WAKEUP_EXT0:
			Serial.println("Wakeup caused by external signad using RTC_IO");
			break;
		case ESP_SLEEP_WAKEUP_EXT1:
			Serial.println("Wakeup caused by external signal using RTC_CNTL");
			break;
		case ESP_SLEEP_WAKEUP_TIMER:
			Serial.println("Wakeup caused by timer");
			break;
		case ESP_SLEEP_WAKEUP_TOUCHPAD:
			Serial.println("Wakeup caused by touchpad");
			break;
		case ESP_SLEEP_WAKEUP_ULP:
			Serial.println("Wakeup caused by ULP program");
			break;

		default:
			Serial.println("Wakeup was not caused by deep sleep: %dn" + wakeUpReason);
			break;
	}
}


void setup() {
	Serial.begin(9600);

	wifiManager = WifiManager();

	wifiCallbacks = new CharacteristicWifiCallbacks(&wifiManager);

	setupBLE(wifiCallbacks);

	timed_event = 1000; // after 1000 ms trigger the event
	current_time = millis();
	start_time = current_time;

  	Monitor.current(PIN_CORRIENTE, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)
	Monitor.voltage(PIN_VOLTAJE, CALIBRATION_VOLTAGE, CALIBRATION_PHASE);

  	sensors = Sensors();



	//configureWifiCallbacks(callbacks);


	//DEEP-SLEEP
  	/*printWakeUpReason();
	Serial.println();
  	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S); //ESP32 wakes up every 5 seconds
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); //All RTC Periferals are powered
	esp_deep_sleep_start();*/
}

void loop() {
	//printWakeUpReason(); //Print the wakeup reason for ESP32

  	current_time = millis(); // update the timer every cycle

	if(current_time - start_time >= timed_event){
		// the event to trigger
		setValueCharacteristic("voltage", std::to_string(sensors.voltageCalculation()));
		setValueCharacteristic("current", std::to_string(sensors.currentCalculation()));
		setValueCharacteristic("power", std::to_string(sensors.powerCalculation()));

		Serial.println("FUNCIONES: ");
		Serial.print("Voltage: ");
		Serial.println(sensors.voltage);
		Serial.print("Current: ");
		Serial.println(sensors.current);
		Serial.print("Power: "); 
		Serial.println(sensors.power);
		Serial.println();
		Serial.println();
		
		//TODO: Need to check if the value is correct:
		interval = std::stoi(getValueCharacteristic("interval"));
		timed_event = interval * 1000;
		start_time = current_time;  // reset the timer

		//Monitor.calcVI(20, 2000);
		//Monitor.serialprint();
	}

	//esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S); //ESP32 wakes up every 5 seconds

	//esp_light_sleep_start();
}