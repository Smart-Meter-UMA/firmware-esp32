#include "ota_updater.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>



CharacteristicOTACallbacks::CharacteristicOTACallbacks(OTAUpdater* OTAUpdaterInstance) {
    otaUpdater = OTAUpdaterInstance;
}

// If it's writing, it must be the smartphone is sending information about the ssid or password:
void CharacteristicOTACallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{ 
    if (pCharacteristic->getUUID().toString() == UUID_UPDATEURL)
    {
        // It's the UpdatedURL:
        String urlExtracted = String(pCharacteristic->getValue().c_str());
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(urlExtracted);

        otaUpdater->setFirmwareUrl(urlExtracted.c_str());  
    }
}

OTAUpdater::OTAUpdater() {
    firmwareUrl = "";
    fileSystemUrl = "";
}

void OTAUpdater::setFirmwareUrl(const char* url) {
    firmwareUrl = url;
}

void OTAUpdater::setFileSystemUrl(const char* url) {
    fileSystemUrl = url;
}

bool OTAUpdater::performUpdate(int command, const char* updateUrl) {
    if (strlen(updateUrl) == 0) {
        Serial.println("Error: Update URL is blank.");
        return false;
    }

    HTTPClient http;
    http.begin(updateUrl);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        bool canBegin = Update.begin(contentLength, command);
        
        if (canBegin) {
            WiFiClient *client = http.getStreamPtr();
            size_t written = Update.writeStream(*client);

            if (written == contentLength) {
                Serial.println("Written: " + String(written) + " successfully.");
            } else {
                Serial.println("Written: " + String(written) + "/" + String(contentLength) + ". Error!");
                return false;
            }

            if (Update.end()) {
                Serial.println("Update done!");
                if (Update.isFinished()) {
                    Serial.println("Update was successful.");
                    http.end();
                    return true;
                } else {
                    Serial.println("Update was not successful.");
                    return false;
                }
            } else {
                Serial.println("Error #: " + String(Update.getError()));
                return false;
            }
        } else {
            Serial.println("Update could not begin. Not enough space to begin.");
            return false;
        }
    } else {
        Serial.println("HTTP Error: " + String(httpCode));
        return false;
    }

    http.end();
}

void OTAUpdater::updateAll() {
    if (strlen(firmwareUrl) == 0) {
        Serial.println("Error: Firmware URL is blank.");
        return;
    }

    if (performUpdate(U_FLASH, firmwareUrl)) {
        Serial.println("Firmware update done.");
        if (strlen(fileSystemUrl) == 0) {
            Serial.println("Error: Filesystem URL is blank.");
            return;
        }
        if (performUpdate(U_SPIFFS, fileSystemUrl)) {
            Serial.println("File system update done. Rebooting...");
            ESP.restart();
        } else {
            Serial.println("Error updating file system.");
        }
    } else {
        Serial.println("Error updating firmware.");
    }
}

void OTAUpdater::updateCombined() {
    if (strlen(firmwareUrl) == 0 || strlen(fileSystemUrl) == 0) {
        Serial.println("Error: Firmware URL is blank.");
        return;
    }

    if (performUpdate(U_FLASH, firmwareUrl)) {
        Serial.println("Combined update done. Rebooting...");
        ESP.restart();
    } else {
        Serial.println("Error updating combined firmware and file system.");
    }
}

void OTAUpdater::updateFirmware() {
    if (strlen(firmwareUrl) == 0) {
        Serial.println("Error: Firmware URL is blank.");
        return;
    }

    if (performUpdate(U_FLASH, firmwareUrl)) {
        Serial.println("Firmware update done. Rebooting...");
        ESP.restart();
    } else {
        Serial.println("Error updating firmware.");
    }
}

void OTAUpdater::updateFileSystem() {
    if (strlen(fileSystemUrl) == 0) {
        Serial.println("Error: Filesystem URL is blank.");
        return;
    }

    if (performUpdate(U_SPIFFS, fileSystemUrl)) {
        Serial.println("File system update done. Rebooting...");
        ESP.restart();
    } else {
        Serial.println("Error updating file system.");
    }
}
