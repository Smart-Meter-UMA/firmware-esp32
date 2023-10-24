#include "ble.h"

NimBLEServer *pServer;
NimBLEService *pService;
NimBLECharacteristic *pVoltageCharac;
NimBLECharacteristic *pIntensityCharac;
NimBLECharacteristic *pIntervalCharac;
NimBLECharacteristic *pPowerCharac;
NimBLECharacteristic *pTokenCharac;
NimBLEAdvertising *pAdvertising;

#ifdef WIFI_BLE_ENABLED
NimBLECharacteristic *pSSIDCharac;
NimBLECharacteristic *pPasswordCharac;
NimBLECharacteristic *pStatusWifiCharac;
#endif

void ServerCallbacks::onConnect(NimBLEServer *pServer)
{
    Serial.println("Client connected");
    Serial.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
};
void ServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
    Serial.print("Client address: ");
    Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    /** We can use the connection handle here to ask for different connection parameters.
     *  Args: connection handle, min connection interval, max connection interval
     *  latency, supervision timeout.
     *  Units; Min/Max Intervals: 1.25 millisecond increments.
     *  Latency: number of intervals allowed to skip.
     *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
     */
    pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
};

void ServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
    Serial.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
};
void ServerCallbacks::onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
{
    Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
};

/** Handler class for characteristic actions */

void CharacteristicCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
};

void CharacteristicCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
};
/** Called before notification or indication is sent,
 *  the value can be changed here before sending if desired.
 */
void CharacteristicCallbacks::onNotify(NimBLECharacteristic *pCharacteristic)
{
    Serial.println("Sending notification to clients");
};

/** The status returned in status is defined in NimBLECharacteristic.h.
 *  The value returned in code is the NimBLE host return code.
 */
void CharacteristicCallbacks::onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
{
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    Serial.println(str);
};

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
{
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0)
    {
        str += " Unsubscribed to ";
    }
    else if (subValue == 1)
    {
        str += " Subscribed to notifications for ";
    }
    else if (subValue == 2)
    {
        str += " Subscribed to indications for ";
    }
    else if (subValue == 3)
    {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    Serial.println(str);
};

/** Handler class for descriptor actions */

void DescriptorCallbacks::onWrite(NimBLEDescriptor *pDescriptor){
    // std::string dscVal = (std::string)pDescriptor->getValue();
    // Serial.print("Descriptor witten value:");
    // Serial.println(dscVal);
};

void DescriptorCallbacks::onRead(NimBLEDescriptor *pDescriptor)
{
    Serial.print(pDescriptor->getUUID().toString().c_str());
    Serial.println(" Descriptor read");
};

void configureWifiCallbacks(CharacteristicWifiCallbacks *wifiCallbacks){
    #ifdef WIFI_BLE_ENABLED
        pSSIDCharac = pService->createCharacteristic(UUID_SSID, NIMBLE_PROPERTY::WRITE);
        pSSIDCharac->setCallbacks(wifiCallbacks);

        pPasswordCharac = pService->createCharacteristic(UUID_PASSWORD, NIMBLE_PROPERTY::WRITE);
        pPasswordCharac->setCallbacks(wifiCallbacks);

        pStatusWifiCharac = pService->createCharacteristic(UUID_STATUSWIFI, NIMBLE_PROPERTY::READ);
        pStatusWifiCharac->setCallbacks(wifiCallbacks);
    #endif
}

void setupBLE(CharacteristicWifiCallbacks *wifiCallbacks)
{
    NimBLEDevice::init("SmartMeter");
    NimBLEDevice::setMTU(527);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    pService = pServer->createService(UUID_SERVICE);
    
    // Characteristic for Intensity (Only read):
    pIntensityCharac = pService->createCharacteristic(UUID_INTENSITY, NIMBLE_PROPERTY::READ);
    pIntensityCharac->setCallbacks(new CharacteristicCallbacks());
    // Characteristic for Voltage (Only read):
    pVoltageCharac = pService->createCharacteristic(UUID_VOLTAGE, NIMBLE_PROPERTY::READ);
    pVoltageCharac->setCallbacks(new CharacteristicCallbacks());

    // Characteristic for Power (Read, Subscribe):
    pPowerCharac = pService->createCharacteristic(UUID_POWER, NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    pPowerCharac->setCallbacks(new CharacteristicCallbacks());

    // Characteristic for interval (Read/Write)
    pIntervalCharac = pService->createCharacteristic(UUID_INTERVAL, NIMBLE_PROPERTY::READ |
			NIMBLE_PROPERTY::WRITE);
    pIntervalCharac->setCallbacks(new CharacteristicCallbacks());
    pIntervalCharac->setValue("1");

    // Characteristic for Token (Only write)
    pTokenCharac = pService->createCharacteristic(UUID_TOKEN, NIMBLE_PROPERTY::WRITE, 512);
    pTokenCharac->setCallbacks(new CharacteristicCallbacks());
 
    #ifdef WIFI_BLE_ENABLED
        configureWifiCallbacks(wifiCallbacks);
    #endif

    pService->start();

    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(UUID_SERVICE);
    // Appearance: Energy Meter.
    pAdvertising->setAppearance(0x0557);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
}

void disableBLE(){
    NimBLEDevice::deinit(true);
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
}

void setValueCharacteristic(std::string characteristic, std::string value){
    if(characteristic == "voltage"){
        pVoltageCharac->setValue(value);
        return;
    }
    if(characteristic == "intensity"){
        pIntensityCharac->setValue(value);
        return;
    }
    if(characteristic == "power"){
        pPowerCharac->setValue(value);
        pPowerCharac->notify(true);
        return;
    }
    if(characteristic == "interval"){
        pIntervalCharac->setValue(value);
        return;
    }
    if(characteristic == "token"){
        pTokenCharac->setValue(value);
        // Ensure the string is not too long for the buffer
        if (value.length() < sizeof(JWTToken)) {
            // Copy the string content to your char array, token
            strlcpy(JWTToken, value.c_str(), sizeof(JWTToken));
        } else {
            Serial.println("String too long for the buffer of JWTToken");
        }
        return;
    }

}
std::string getValueCharacteristic(std::string characteristic){
    if(characteristic == "voltage"){
        return pVoltageCharac->getValue();
    }
    if(characteristic == "intensity"){
        return pIntensityCharac->getValue();
    }
    if(characteristic == "power"){
        return pPowerCharac->getValue();
    }
    if(characteristic == "interval"){
        return pIntervalCharac->getValue();
    }
    if(characteristic == "token"){
        return pTokenCharac->getValue();
    }
    return "";
}