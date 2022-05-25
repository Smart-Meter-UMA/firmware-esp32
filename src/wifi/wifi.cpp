#include "wifi.h"
#include <Arduino.h>

void setPassword();
void setSSID();
void getStatus();
void setStatus();

NimBLECharacteristic *pSSIDCharac;
NimBLECharacteristic *pPasswordCharac;
NimBLECharacteristic *pStatusWifiCharac;


// If it's reading, it must be that the smartphone is reading the status. Default behaviour.
void CharacteristicWifiCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
};

// If it's writing, it must be the smartphone is sending information about the ssid or password:
void CharacteristicWifiCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{ 
    if (pCharacteristic->getUUID().toString() == UUID_SSID)
    {
        // It's the SSID:
        String ssidExtracted = String(pCharacteristic->getValue().c_str());
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(ssidExtracted);

        Serial.print("SSID extracted: ");
        Serial.println(ssidExtracted);      
    }

    if (pCharacteristic->getUUID().toString() == UUID_PASSWORD)
    {
        // It's the Password:
        String ssidExtracted = String(pCharacteristic->getValue().c_str());
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());

        Serial.print("SSID extracted: ");
        Serial.println(pCharacteristic->getValue().c_str());      
    }
    
}