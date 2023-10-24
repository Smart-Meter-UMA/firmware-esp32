// SmartMeterServer.cpp
#include "smartmeterserver.h"

// NTP Time settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;  // GMT offset in seconds
const int   daylightOffset_sec = 0;  // Daylight saving offset in seconds

SmartMeterServer::SmartMeterServer(const char* serverUrl, char* jwtToken)
    : serverUrl(serverUrl), jwtToken(jwtToken) {
        configured = false;

}


void SmartMeterServer::configNTPTime() {
    // Configure time synchronization via NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

bool SmartMeterServer::isJWTConfigured() {
    return (!jwtToken || *jwtToken == '\0');
}

bool SmartMeterServer::deviceVerification() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error in WiFi connection.");
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();

    Serial.print("Connecting to ");
    Serial.println(serverUrl);
    if (!client.connect(serverUrl, 443)) {
        Serial.println("Connection to server failed!");
        return false;
    }

    String url = String(serverUrl) + "/verificacionDispositivo";
    String httpRequest = String("POST ") + url + " HTTP/1.1\r\n" +
                         "Host: " + serverUrl + "\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Authorization: Bearer " + jwtToken + "\r\n" +
                         "Content-Length: 0\r\n" +
                         "\r\n";

    client.print(httpRequest);

    String statusLine = client.readStringUntil('\r');
    client.readStringUntil('\n');

    int statusCode = statusLine.substring(9, 12).toInt();
    Serial.print("Status code: ");
    Serial.println(statusCode);

    while (client.connected()) {
        String header = client.readStringUntil('\n');
        if (header == "\r") {
            break;
        }
    }

    if (statusCode == 200 || statusCode == 201) {
        Serial.println("Verified successfully.");
        return true;
    } else {
        Serial.print("Verification failed. Status code: ");
        Serial.println(statusCode);
        return false;
    }
}

bool SmartMeterServer::initialConfig(){
    configNTPTime();
    if(deviceVerification()){
        Serial.println("Device verified.");
        return true;
    }
    return false;
}

bool SmartMeterServer::sendMeasurements(char dates[][MAX_STRING_DATE], char voltages[][MAX_STRING_DOUBLE], char intensity[][MAX_STRING_DOUBLE], int32_t measurements_taken) {
    if (measurements_taken <= 0) {
        Serial.println("No measurements to send");
        return false;
    }

    WiFiClientSecure client;

    client.setInsecure();

    // Attempt to connect to the server
    Serial.print("Connecting to server: ");
    Serial.println(this->serverUrl);
    if (!client.connect(this->serverUrl, 443)) { // HTTPS port is 443
        Serial.println("Connection to server failed");
        return false;
    }

    // Create the JSON document with measurements
    DynamicJsonDocument doc(1024 * 2); // adjust the capacity as needed
    JsonArray array = doc.to<JsonArray>();
    for (int i = 0; i < measurements_taken; i++) {
        JsonObject measurement = array.createNestedObject();
        measurement["fecha"] = dates[i];
        measurement["intensidad"] = atof(intensity[i]);
        measurement["voltaje"] = atof(voltages[i]);
    }

    // Convert JSON to String
    String jsonData;
    serializeJson(doc, jsonData);

    // Create the POST request
    String httpRequest = String("POST ") + "/kproject/medidas" + " HTTP/1.1\r\n" +
                         "Host: " + this->serverUrl + "\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Content-Length: " + jsonData.length() + "\r\n" +
                         "Authorization: Bearer " + this->jwtToken + "\r\n\r\n" +
                         jsonData + "\r\n";

    // Send the POST request
    client.print(httpRequest);

    // Check the HTTP response
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }

    // Read the HTTP response body
    String responseBody = client.readString();

    // Close the connection
    client.stop();

    // Here, you should parse the response body to check the status of your POST request
    // This is just a very basic way to check success, you might want to adjust based on the actual response content
    if (responseBody.indexOf("200 OK") != -1 || responseBody.indexOf("201 Created") != -1) {
        Serial.println("Measurements sent successfully");
        return true;
    } else {
        Serial.println("Failed to send measurements");
        return false;
    }
}

void SmartMeterServer::setToken(const char *token){
    if (token == nullptr) {
            jwtToken[0] = '\0';
            return;
        }

        jwtToken = new char[strlen(token) + 1];
        strlcpy(jwtToken, token, sizeof(jwtToken));
}