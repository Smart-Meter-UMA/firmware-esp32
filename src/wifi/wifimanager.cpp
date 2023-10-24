#include "wifimanager.h"
#include <Arduino.h>
#include "WiFi.h"

extern NimBLECharacteristic *pSSIDCharac;
extern NimBLECharacteristic *pPasswordCharac;
extern NimBLECharacteristic *pStatusWifiCharac;

CharacteristicWifiCallbacks::CharacteristicWifiCallbacks(WifiManager* wifiManagerInstance) {
    wifiManager = wifiManagerInstance;
}

// If it's reading, it must be that the smartphone is reading the status. Default behaviour.
void CharacteristicWifiCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    if (pCharacteristic->getUUID().toString() == UUID_SSID)
    {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
        // Set up WiFi in station mode and disconnect from any network
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        int n = WiFi.scanNetworks();
        if (n == 0) {
            Serial.println("No networks found");
        } else {
            Serial.print(n);
            Serial.println(" networks found");
            for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (RSSI: ");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
        }
        Serial.println("");
    }

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
        
        wifiManager->setPassword(passExtracted.c_str());       
    }
    
}

WifiManager::WifiManager() : _ssid(""), _password(""), _ssid_isSet(false), _pass_isSet(false) {
    _ssid[0] = '\0';         // This sets the first character to the null terminator, making it an empty string.
    _password[0] = '\0'; 
}

void WifiManager::setSSID(const char* ssid) {
    Serial.println(String("SSID entered ") + String(ssid));
    strncpy(_ssid, ssid, sizeof(_ssid) - 1);
    _ssid[sizeof(_ssid) - 1] = '\0';  // Ensure null-termination
    _ssid_isSet = true;
        if(!_pass_isSet){
        Serial.println(String("Expecting password."));
    }
}

void WifiManager::setPassword(const char* password) {
    Serial.println(String("Password entered ") + String(password));
    strncpy(_password, password, sizeof(_password) - 1);
    _password[sizeof(_password) - 1] = '\0';  // Ensure null-termination
    _pass_isSet = true;
    if(!_ssid_isSet){
        Serial.println(String("Expecting ssid."));
    } else {
        connectToNetwork();
    }
}

bool WifiManager::connectToNetwork() {
    Serial.println("Connecting wifi with ssid " + String(_ssid) + " and password: " + String(_password));
    int tries = 0;
    if ((!_ssid_isSet && _ssid[0] != '\0') || (!_pass_isSet  && _password[0] != '\0' > 0)) {
        // Handle the case where SSID or password is not set
        Serial.println("Error getting ssid and password");
        return false;
    }

    WiFi.begin(_ssid, _password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); // Wait 500ms
        //Serial.println("Waiting connection...");
        tries++;
        Serial.println(WiFi.status());
        if(tries > 20){
            return false;
            Serial.println("Error connecting to Wifi");
                        // Detailed error reason.
            switch (WiFi.status()) {
                case WL_NO_SSID_AVAIL:
                    Serial.println("Network SSID unavailable. Please check if the SSID is correct or if the network is within range.");
                    break;
                case WL_CONNECT_FAILED:
                    Serial.println("Failed to connect. Please ensure the password is correct and try again.");
                    break;
                case WL_IDLE_STATUS:
                    Serial.println("WiFi module is idle and not attempting to connect. Please check if the WiFi module is active and initialized.");
                    break;
                case WL_DISCONNECTED:
                    Serial.println("Disconnected from the network. Please check if the network is active and within range.");
                    break;
                // Add other error codes relevant to your ESP32's WiFi library version.
                default:
                    Serial.println("An unknown error occurred while trying to connect to WiFi.");
            }
            return false; // Return false to indicate failure
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
    info += "Wifi " + String("on channel ") + String(WiFi.channel());
    info += "IP Address: " + WiFi.localIP().toString() + "\n";
    info += "Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm\n";
    return info;
}

// void WifiManager::configureHttpsClient() {

// }