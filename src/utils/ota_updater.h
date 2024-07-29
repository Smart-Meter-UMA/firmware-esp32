#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>

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

#endif