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

        wifiManager->setSSID(ssidExtracted.c_str());  
    }

    if (pCharacteristic->getUUID().toString() == UUID_PASSWORD)
    {
        // It's the Password:
        String passExtracted = String(pCharacteristic->getValue().c_str());
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
        
        wifiManager->setSSID(passExtracted.c_str());       
    }
    
}

WifiManager::WifiManager() : _ssid(""), _password(""), _ssid_isSet(false), _pass_isSet(false) {
    // Other initialization as necessary
}

void WifiManager::setSSID(const char* ssid) {
    Serial.println(String("SSID entered ") + String(ssid));
    _ssid = ssid;
    _ssid_isSet = true;
        if(!_pass_isSet){
        Serial.println(String("Expecting password."));
    }
}

void WifiManager::setPassword(const char* password) {
    Serial.println(String("Password entered ") + String(password));
    _pass_isSet = true;
    _password = password;
    if(!_ssid_isSet){
        Serial.println(String("Expecting ssid."));
    } else {
        connectToNetwork();
    }
}

bool WifiManager::connectToNetwork() {
    int tries = 0;
    if ((!_ssid_isSet && _ssid.length() > 0) || (!_pass_isSet  && _password.length() > 0)) {
        // Handle the case where SSID or password is not set
        return false;
    }

    WiFi.begin(_ssid.c_str(), _password.c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); // Wait 500ms
        Serial.println("Waiting connection...");
        tries++;
        if(tries > 5){
            return false;
        }
        // Here you could add some timeout logic to prevent infinite loops
    }

    Serial.println("WIFI CONNECTED!");
    getWifiInfo();

    // configureHttpsClient(); // Configure HTTPS client after successful connection
    return true; // Connected successfully
}

// WiFiClientSecure WifiManager::getHttpsClient() {
//     return _httpsClient;
// }

String WifiManager::getWifiInfo() {
    String info = "SSID: " + String(WiFi.SSID()) + "\n";
    info += "Wifi Encryption Type: " + String(WiFi.encryptionType()) + String("on channel") + String(WiFi.channel());
    info += "IP Address: " + WiFi.localIP().toString() + "\n";
    info += "Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm\n";
    return info;
}

// void WifiManager::configureHttpsClient() {

// }