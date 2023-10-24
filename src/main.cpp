#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ble/ble.h>
#include <sensors/sensors.h>
#include <wifi/wifimanager.h>
#include "time.h"
#include "WiFiClientSecure.h"
#include "smartmeterserver/smartmeterserver.h"

#define CONFIG_NIMBLE_CPP_DEBUG_LEVEL 4
#define ESP32 1

//DEEP-SLEEP
//#define uS_TO_S 1000000 //Conversion factor for micro seconds to seconds
//#define TIME_TO_SLEEP 5 //Time ESP32 will go to sleep (in seconds)

//LIGHT-SLEEP
#define uS_TO_S 1000000ULL //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 5 //Time ESP32 will go to sleep (in seconds)

#define NUM_MAX_MEASUREMENTS 20
#define MAX_STRING_DATE 30
#define MAX_STRING_DOUBLE 10


const char* server = "smart-meter-backend.ertis.uma.es";

uint32_t current_time;
uint32_t start_time;
uint32_t timed_event;

uint32_t transmission_start_time;
uint32_t transmission_timed_event;

uint32_t interval;

Sensors sensors;

EnergyMonitor Monitor;

WifiManager wifiManager;

CharacteristicWifiCallbacks *wifiCallbacks;

bool attachedToServer;

SmartMeterServer *smartMeterServer;

char JWTToken[150];

// Measurements store:
char dates[NUM_MAX_MEASUREMENTS][MAX_STRING_DATE];
char voltages[NUM_MAX_MEASUREMENTS][MAX_STRING_DOUBLE];
char intensity[NUM_MAX_MEASUREMENTS][MAX_STRING_DOUBLE];
int32_t measurements_taken;

void printLocalTime()
{
	struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
    }

    char timeStringBuff[80]; // holds the string formed by the strftime function

    // Formatting the time structure in ISO 8601 format. This format is represented by the
    // string: "YYYY-MM-DDTHH:MM:SS", where 'T' is just a literal to separate the date from the time.
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);

    // Printing out the time string that's now in ISO 8601 format
    Serial.println(timeStringBuff);
}

String getISOTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return String("");  
    }

    char timeStringBuff[25]; 

    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%SZ", &timeinfo); 

    return String(timeStringBuff);
}

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

  	//Monitor.current(PIN_CORRIENTE, CALIBRATION_CURRENT); //ESP's PIN and calibration's valor (this valor is teoric)
	//onitor.voltage(PIN_VOLTAJE, CALIBRATION_VOLTAGE, CALIBRATION_PHASE);

  	sensors = Sensors();

	attachedToServer = false;

	SmartMeterServer smartMeterServer("smart-meter-backend.ertis.uma.es", "");

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

		double voltage = sensors.voltageCalculation();
		double current = sensors.currentCalculation();
		double power = sensors.powerCalculation();
		// the event to trigger
		setValueCharacteristic("voltage", std::to_string(voltage));
		setValueCharacteristic("current", std::to_string(current));
		setValueCharacteristic("power", std::to_string(power));

		Serial.println("FUNCIONES: ");
		Serial.print("Voltage: ");
		Serial.println(voltage);
		Serial.print("Current: ");
		Serial.println(current);
		Serial.print("Power: "); 
		Serial.println(power);
		Serial.println();
		Serial.println();

		String currentTimeISO = getISOTime();
		if (currentTimeISO.length() > 0) { // Check it's not empty (which means time was retrieved successfully)
			strncpy(dates[0], currentTimeISO.c_str(), sizeof(dates[0]) - 1);
			dates[measurements_taken][sizeof(dates[0]) - 1] = '\0';  // Ensure null termination
			strncpy(dates[0], currentTimeISO.c_str(), sizeof(dates[0]) - 1);

			snprintf(voltages[measurements_taken], sizeof(voltages[measurements_taken]), "%.2f", voltage);  // format as fixed decimal, 2 places
			snprintf(intensity[measurements_taken], sizeof(intensity[measurements_taken]), "%.2f", intensity);

			measurements_taken++;
			
		} else {
			Serial.println("Can't save the measurement. Error date.");
		}
		
		//TODO: Need to check if the value is correct:
		interval = std::stoi(getValueCharacteristic("interval"));
		timed_event = interval * 1000;
		start_time = current_time;  // reset the timer

		//Monitor.calcVI(20, 2000);
		//Monitor.serialprint();
	}

	if(WiFi.status() == WL_CONNECTED){
		if(!smartMeterServer->isJWTConfigured()){
			smartMeterServer->setToken(JWTToken);
		}
		if(current_time - transmission_start_time >= transmission_timed_event){
			if(attachedToServer){
				smartMeterServer->sendMeasurements(dates,voltages,intensity,measurements_taken);
				
			} else {
				attachedToServer = smartMeterServer->initialConfig();
			}
			transmission_start_time = current_time;
		}

	}

	//esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S); //ESP32 wakes up every 5 seconds

	//esp_light_sleep_start();
}