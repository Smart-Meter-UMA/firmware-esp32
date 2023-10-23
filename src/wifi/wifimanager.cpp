#include "wifimanager.h"
#include <Arduino.h>
#include "WiFi.h"

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

WifiManager::WifiManager() : _ssid(nullptr), _password(nullptr) {
    // Initialization if needed
}

void WifiManager::setSSID(const char* ssid) {
    _ssid = ssid;
}

void WifiManager::setPassword(const char* password) {
    _password = password;
}

bool WifiManager::connectToNetwork() {
    if (!_ssid || !_password) {
        // Handle the case where SSID or password is not set
        return false;
    }

    WiFi.begin(_ssid, _password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); // Wait 500ms
        // Here you could add some timeout logic to prevent infinite loops
    }

    configureHttpsClient(); // Configure HTTPS client after successful connection
    return true; // Connected successfully
}

WiFiClientSecure WifiManager::getHttpsClient() {
    return _httpsClient;
}

String WifiManager::getWifiInfo() {
    String info = "SSID: " + String(WiFi.SSID()) + "\n";
    info += "Wifi Encryption Type: " + String(WiFi.encryptionType()) + String("on channel") + String(WiFi.channel()))
    info += "IP Address: " + WiFi.localIP().toString() + "\n";
    info += "Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm\n";
    return info;
}

void WifiManager::configureHttpsClient() {

}