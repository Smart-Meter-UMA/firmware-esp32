#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensors.h"

// Mock class for EnergyMonitor to simulate its behavior
class MockEnergyMonitor {
public:
    MOCK_METHOD1(current, void(int pin, double calibration));
    MOCK_METHOD3(voltage, void(int pin, double calibration, double phaseShift));
    MOCK_METHOD1(calcIrms, double(int numberOfSamples));
    MOCK_METHOD0(readVcc, double());
};

// Global mock object instance
MockEnergyMonitor* mockMonitor = nullptr;

// Override the global `Monitor` instance with the mock
EnergyMonitor Monitor = *mockMonitor;

// Test fixture for Sensors class
class SensorsTest : public ::testing::Test {
protected:
    Sensors* sensors;

    void SetUp() override {
        mockMonitor = new MockEnergyMonitor();  // Create the mock instance
        sensors = new Sensors();  // Create Sensors instance
    }

    void TearDown() override {
        delete sensors;  // Clean up the Sensors instance
        delete mockMonitor;  // Clean up the mock instance
        mockMonitor = nullptr;
    }
};

// Test the initialization of the Sensors class
TEST_F(SensorsTest, InitializationTest) {
    EXPECT_CALL(*mockMonitor, current(PIN_CURRENT, CALIBRATION_CURRENT)).Times(1);
    EXPECT_CALL(*mockMonitor, voltage(PIN_VOLTAGE, CALIBRATION_VOLTAGE, CALIBRATION_PHASE)).Times(1);

    Sensors sensorsInstance;
}

// Test for current calculation
TEST_F(SensorsTest, CurrentCalculationTest) {
    // Set expectation for calcIrms to return a specific value
    EXPECT_CALL(*mockMonitor, calcIrms(CALIBRATION_IRMS))
        .WillOnce(testing::Return(5.6));  // Mock calcIrms to return 5.6

    // Test the current calculation, where the mock should return 5.6
    double current = sensors->currentCalculation();
    EXPECT_NEAR(current, 5.6 - 0.6, 0.001);  // Expect the calculation to return abs(5.6 - 0.6)
}

// Test for voltage calculation without fixed voltage
TEST_F(SensorsTest, VoltageCalculationWithoutFixedVoltageTest) {
    // Set expectation for readVcc to return a specific value
    EXPECT_CALL(*mockMonitor, readVcc())
        .WillOnce(testing::Return(220.0));  // Mock readVcc to return 220.0

    // Test voltage calculation
    double voltage = sensors->voltageCalculation();
    EXPECT_NEAR(voltage, 220.0, 0.001);  // Expect the calculation to return 220.0
}

// Test for voltage calculation with fixed voltage (macro defined)
#ifdef USE_FIXED_VOLTAGE
TEST_F(SensorsTest, VoltageCalculationWithFixedVoltageTest) {
    double voltage = sensors->voltageCalculation();
    EXPECT_NEAR(voltage, FIXED_VOLTAGE, 0.001);  // Expect to return fixed voltage
}
#endif

// Test for power calculation
TEST_F(SensorsTest, PowerCalculationTest) {
    // Mock both current and voltage calculations
    EXPECT_CALL(*mockMonitor, calcIrms(CALIBRATION_IRMS))
        .WillOnce(testing::Return(5.6));  // Mock current to return 5.6
    EXPECT_CALL(*mockMonitor, readVcc())
        .WillOnce(testing::Return(220.0));  // Mock voltage to return 220.0

    // Test power calculation
    double power = sensors->powerCalculation();
    EXPECT_NEAR(power, (5.6 - 0.6) * 220.0, 0.001);  // Expect power = current * voltage
}

// Test for fake data generation
TEST_F(SensorsTest, FakeDataTest) {
    sensors->fakeData();
    EXPECT_GT(sensors->current, 0);  // Fake data should result in non-zero current
    EXPECT_GT(sensors->voltage, 0);  // Fake data should result in non-zero voltage
    EXPECT_GT(sensors->power, 0);    // Fake data should result in non-zero power
}
