#ifndef BLE_H
#define BLE_H
#include <NimBLEDevice.h>
#include <Arduino.h>
#include "wifi/wifimanager.h"

#define UUID_SERVICE "534d4d54-d32a-11ec-9d64-0242ac120002"

// UUIDs for sensors:
#define UUID_VOLTAGE "56303030-d32a-11ec-9d64-0242ac120002"
#define UUID_INTENSITY "49303030-d32a-11ec-9d64-0242ac120002"
#define UUID_POWER "50303030-d32a-11ec-9d64-0242ac120002"
#define UUID_INTERVAL "4956414c-d32a-11ec-9d64-0242ac120002"

// UUIDs for configuration:1
#define UUID_TOKEN "544f4b4e-d32a-11ec-9d64-0242ac120002"

#ifdef WIFI_BLE_ENABLED
#include "wifi/wifimanager.h"
#endif

#define LED 23
#define ONBOARD_LED 2
#define USE_SERIAL_BLE 1

extern NimBLEServer *pServer;
extern NimBLEService *pService;
extern NimBLECharacteristic *pVoltageCharac;
extern NimBLECharacteristic *pIntensityCharac;
extern NimBLECharacteristic *pIntervalCharac;
extern NimBLECharacteristic *pPowerCharac;
extern NimBLECharacteristic *pTokenCharac;
extern NimBLEAdvertising *pAdvertising;
extern char JWTToken[150];

class ServerCallbacks: public NimBLEServerCallbacks{
    void onConnect(NimBLEServer* pServer);
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
    void onDisconnect(NimBLEServer* pServer);
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc);
};

class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic);
    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onNotify(NimBLECharacteristic* pCharacteristic);
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code);
    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue);
};

class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor* pDescriptor);
    void onRead(NimBLEDescriptor* pDescriptor);
};

void setupBLE(CharacteristicWifiCallbacks *wifiCallbacks);
void disableBLE();

void configureWifiCallbacks(CharacteristicWifiCallbacks *wificallbacks);

void setValueCharacteristic(std::string characteristic, std::string value);
std::string getValueCharacteristic(std::string characteristic);

#endif