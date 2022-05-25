#ifndef WIFI_H
#define WIFI_H

#define UUID_SSID "6bfe5343-d32a-11ec-9d64-0242ac120002"
#define UUID_PASSWORD "760a51b2-d32a-11ec-9d64-0242ac120002"
#define UUID_STATUSWIFI "8bdb959e-d32a-11ec-9d64-0242ac120002"

#include <NimBLEDevice.h>

extern NimBLECharacteristic *pSSIDCharac;
extern NimBLECharacteristic *pPasswordCharac;
extern NimBLECharacteristic *pStatusWifiCharac;

// This characteristics are use for the WIFI callbacks and are linked with the WiFi Module.
class CharacteristicWifiCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic);
    void onWrite(NimBLECharacteristic* pCharacteristic);
};

#endif