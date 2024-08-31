#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include <NimBLEDevice.h>

#define UUID_UPDATEURL "4f544155-d32a-11ec-9d64-0242ac120002"

extern NimBLECharacteristic *pSSIDCharac;

class OTAUpdater {
public:
  OTAUpdater();
  void setFirmwareUrl(const char* url);
  void setFileSystemUrl(const char* url); 
  void updateFirmware();
  void updateFileSystem();
  void updateAll();
  void updateCombined();
  
private:
  const char* firmwareUrl;
  const char* fileSystemUrl;
  bool performUpdate(int command, const char* updateUrl);
};

class CharacteristicOTACallbacks: public NimBLECharacteristicCallbacks {
    private:
        OTAUpdater *otaUpdater;  // pointer to WifiManager instance
    public:
        CharacteristicOTACallbacks(OTAUpdater* OTAUpdaterInstance);
        void onRead(NimBLECharacteristic* pCharacteristic);
        void onWrite(NimBLECharacteristic* pCharacteristic);
};

#endif