#include <gtest/gtest.h>
#include "ota_updater.h"
#include <string>

// Mock classes for HTTPClient, WiFiClient, and Update
class MockHTTPClient {
public:
    MockHTTPClient() = default;

    MOCK_METHOD0(begin, void(const char*));
    MOCK_METHOD0(GET, int());
    MOCK_METHOD0(getSize, int());
    MOCK_METHOD0(getStreamPtr, WiFiClient*());
    MOCK_METHOD0(end, void());
};

class MockWiFiClient {
public:
    size_t write(const uint8_t *buf, size_t size) {
        return size; 
    }
};

class MockUpdate {
public:
    static bool begin(size_t size, int command) {
        return true; 
    }
    
    static size_t writeStream(MockWiFiClient &client) {
        return client.write(nullptr, 1024); 
    }
    
    static bool end() {
        return true; 
    }
    
    static bool isFinished() {
        return true; 
    }
    
    static int getError() {
        return 0; 
    }
};


MockHTTPClient mockHTTPClient;
MockWiFiClient mockWiFiClient;
MockUpdate mockUpdate;


class OTAUpdaterTest : public ::testing::Test {
protected:
    OTAUpdater otaUpdater;

    void SetUp() override {
        
    }

    void TearDown() override {
        
    }
};

TEST_F(OTAUpdaterTest, EmptyFirmwareUrl) {
    otaUpdater.setFirmwareUrl(""); 
    testing::internal::CaptureStdout();
    otaUpdater.updateFirmware(); 
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Error: Firmware URL is blank.") != std::string::npos);
}


TEST_F(OTAUpdaterTest, ValidFirmwareUpdate) {
    otaUpdater.setFirmwareUrl("http://valid.url/firmware.bin");

    
    EXPECT_CALL(mockHTTPClient, GET())
        .WillOnce(testing::Return(HTTP_CODE_OK));
    EXPECT_CALL(mockHTTPClient, getSize())
        .WillOnce(testing::Return(1024)); 


    EXPECT_CALL(mockUpdate, begin(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockUpdate, writeStream(testing::_))
        .WillOnce(testing::Return(1024)); 
    EXPECT_CALL(mockUpdate, end())
        .WillOnce(testing::Return(true));
    EXPECT_CALL(mockUpdate, isFinished())
        .WillOnce(testing::Return(true));

    testing::internal::CaptureStdout();
    otaUpdater.updateFirmware();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(output.find("Firmware update done.") != std::string::npos);
}


TEST_F(OTAUpdaterTest, FailedHTTPUpdate) {
    otaUpdater.setFirmwareUrl("http://invalid.url/firmware.bin");


    EXPECT_CALL(mockHTTPClient, GET())
        .WillOnce(testing::Return(404));

    testing::internal::CaptureStdout();
    otaUpdater.updateFirmware(); 
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(output.find("HTTP Error: 404") != std::string::npos);
}

TEST_F(OTAUpdaterTest, EmptyFileSystemUrl) {
    otaUpdater.setFileSystemUrl(""); 
    testing::internal::CaptureStdout();
    otaUpdater.updateFileSystem(); 
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Error: Filesystem URL is blank.") != std::string::npos);
}