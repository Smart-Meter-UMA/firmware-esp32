// SmartMeterServer.h
#ifndef SMART_METER_SERVER_H
#define SMART_METER_SERVER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "ArduinoJson.h"  // Make sure to have the ArduinoJson library installed.

#define NUM_MAX_MEASUREMENTS 10  // Define this with the appropriate number.
#define MAX_STRING_DATE 30
#define MAX_STRING_DOUBLE 10

class SmartMeterServer {
public:
    SmartMeterServer(const char* serverUrl, char* jwtToken);

    void setToken(const char* token);
    bool isJWTConfigured();
    bool initialConfig();
    bool sendMeasurements(char dates[][MAX_STRING_DATE], char voltages[][MAX_STRING_DOUBLE], char intensity[][MAX_STRING_DOUBLE], int32_t measurements_taken);

private:

    const char* serverUrl;  // To store the server URL
    char* jwtToken;   // To store the JWT token

    WiFiClientSecure httpsClient;  // For secure SSL/TLS communication
    void configNTPTime();
    bool deviceVerification();
    bool configured;
};

#endif //SMART_METER_SERVER_H