#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "smartmeterserver.h"
#include <string>
#include <WiFiClientSecure.h>

// Mock classes for WiFiClientSecure and any other ESP32 specific functions
class MockWiFiClientSecure {
public:
    MockWiFiClientSecure() = default;

    MOCK_METHOD0(connect, bool(const char*, int));
    MOCK_METHOD1(print, size_t(const String&));
    MOCK_METHOD0(connected, bool());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD1(readStringUntil, String(char));
    MOCK_METHOD0(readString, String());
    MOCK_METHOD0(setInsecure, void());
};

// Declare configTime as an external function to mock it
extern void configTime(long gmtOffset_sec, int daylightOffset_sec, const char* ntpServer);

// Create a mock version of configTime using Google Mock
class MockTime {
public:
    MOCK_METHOD3(configTime, void(long, int, const char*));
};

// Create a global instance of the mock
MockTime* mockTimeInstance = nullptr;

// Mock function for `configTime`, as it is ESP32 specific
void configTime(long gmtOffset_sec, int daylightOffset_sec, const char* ntpServer) {
    // Mock implementation of configTime
}

MockWiFiClientSecure mockWiFiClientSecure;

class SmartMeterServerTest : public ::testing::Test {
protected:
    SmartMeterServer* smartMeterServer;
    char jwtToken[256];

    void SetUp() override {
        smartMeterServer = new SmartMeterServer("http://mockserver.com", jwtToken);
        mockTimeInstance = new MockTime();
    }

    void TearDown() override {
        // Cleanup after tests
        delete smartMeterServer;
        delete mockTimeInstance; 
        mockTimeInstance = nullptr;
    }
};

TEST_F(SmartMeterServerTest, ConfigNTPTime_CallsConfigTime) {
    EXPECT_CALL(*mockTimeInstance, configTime(0, 0, "pool.ntp.org"))
        .Times(1);

    smartMeterServer->configNTPTime();
}

TEST_F(SmartMeterServerTest, IsJWTConfigured_WhenTokenIsEmpty) {
    smartMeterServer->setToken("");  // Set an empty token
    EXPECT_TRUE(smartMeterServer->isJWTConfigured());  // Should return true
}

TEST_F(SmartMeterServerTest, ConfigNTPTime) {
    testing::internal::CaptureStdout();  // Capture Serial output
    smartMeterServer->configNTPTime();  // Call the method
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_FALSE(output.empty());  // configTime() should print output related to NTP configuration
}

TEST_F(SmartMeterServerTest, DeviceVerification_WiFiDisconnected) {
    // Assume WiFi is not connected
    EXPECT_CALL(mockWiFiClientSecure, connected())
        .WillOnce(testing::Return(false));  // Simulate WiFi disconnection
    
    testing::internal::CaptureStdout();
    EXPECT_FALSE(smartMeterServer->deviceVerification());  // Should return false due to WiFi disconnection
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Error in WiFi connection.") != std::string::npos);
}

TEST_F(SmartMeterServerTest, DeviceVerification_Success) {
    EXPECT_CALL(mockWiFiClientSecure, connect(testing::_, 443))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, print(testing::_))
        .Times(1);
    EXPECT_CALL(mockWiFiClientSecure, readStringUntil('\r'))
        .WillOnce(testing::Return("HTTP/1.1 200 OK"));
    EXPECT_CALL(mockWiFiClientSecure, connected())
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, readStringUntil('\n'))
        .Times(1);
    EXPECT_CALL(mockWiFiClientSecure, stop())
        .Times(1);

    testing::internal::CaptureStdout();
    EXPECT_TRUE(smartMeterServer->deviceVerification());  // Should return true as the verification succeeds
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Verified successfully.") != std::string::npos);
}


TEST_F(SmartMeterServerTest, DeviceVerification_Failure) {
    EXPECT_CALL(mockWiFiClientSecure, connect(testing::_, 443))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, print(testing::_))
        .Times(1);
    EXPECT_CALL(mockWiFiClientSecure, readStringUntil('\r'))
        .WillOnce(testing::Return("HTTP/1.1 404 Not Found"));
    EXPECT_CALL(mockWiFiClientSecure, connected())
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, readStringUntil('\n'))
        .Times(1);
    EXPECT_CALL(mockWiFiClientSecure, stop())
        .Times(1);

    testing::internal::CaptureStdout();
    EXPECT_FALSE(smartMeterServer->deviceVerification());  // Should return false as verification fails
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Verification failed") != std::string::npos);
}

TEST_F(SmartMeterServerTest, SendMeasurements_Success) {
    char dates[1][MAX_STRING_DATE] = {"2023-09-01 12:00:00"};
    char voltages[1][MAX_STRING_DOUBLE] = {"230.0"};
    char intensity[1][MAX_STRING_DOUBLE] = {"10.0"};

    // Simulate a successful server connection
    EXPECT_CALL(mockWiFiClientSecure, connect(testing::_, 443))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, print(testing::_))
        .Times(1);
    EXPECT_CALL(mockWiFiClientSecure, readStringUntil('\r'))
        .WillOnce(testing::Return("HTTP/1.1 201 Created"));
    EXPECT_CALL(mockWiFiClientSecure, connected())
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockWiFiClientSecure, stop())
        .Times(1);

    testing::internal::CaptureStdout();
    EXPECT_TRUE(smartMeterServer->sendMeasurements(dates, voltages, intensity, 1));  // Should succeed
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Measurements sent successfully") != std::string::npos);
}

TEST_F(SmartMeterServerTest, SendMeasurements_NoMeasurements) {
    char dates[1][MAX_STRING_DATE] = {};
    char voltages[1][MAX_STRING_DOUBLE] = {};
    char intensity[1][MAX_STRING_DOUBLE] = {};

    testing::internal::CaptureStdout();
    EXPECT_FALSE(smartMeterServer->sendMeasurements(dates, voltages, intensity, 0));  // Should fail due to no measurements
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("No measurements to send") != std::string::npos);
}