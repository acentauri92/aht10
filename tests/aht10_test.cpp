#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

extern "C" 
{
    #include "aht10.h"
    #include "i2c_hal.h"

    int8_t i2c_write(uint8_t addr, const uint8_t* data, size_t len) {
        return mock().actualCall("i2c_write")
                     .withParameter("addr", addr)
                     .withMemoryBufferParameter("data", data, len)
                     .withParameter("len", len)
                     .returnIntValue();
    }

    int8_t i2c_read(uint8_t addr, uint8_t* data, size_t len) {
        return mock().actualCall("i2c_read")
                     .withParameter("addr", addr)
                     .withOutputParameter("data", data)
                     .withParameter("len", len)
                     .returnIntValue();
    }

    void delay_ms(uint32_t ms) {
        mock().actualCall("delay_ms")
              .withParameter("ms", ms);
    }

}


//  Test Group

TEST_GROUP(Aht10Driver_TestGroup)
{
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    } 
};

TEST(Aht10Driver_TestGroup, Ah10Init_Success) {
    uint8_t soft_reset_command[1] = {0xBA};
    // 0xE1 (command), 0x08 (calibration enabled), 0x00 (reserved)
    uint8_t init_command[3] = {0xE1, 0x08, 0x00};

    mock().expectOneCall("i2c_write")
          .withParameter("addr", AHT10_I2C_ADDR)
          .withMemoryBufferParameter("data", soft_reset_command, sizeof(soft_reset_command))
          .withParameter("len", 1)
          .andReturnValue(0);

    mock().expectOneCall("delay_ms")
          .withParameter("ms", AHT10_SOFT_RESET_DELAY_MS);

    mock().expectOneCall("i2c_write")
          .withParameter("addr", AHT10_I2C_ADDR)
          .withMemoryBufferParameter("data", init_command, sizeof(init_command))
          .withParameter("len", 3)
          .andReturnValue(0);

    int result = ah10_init();

    LONGS_EQUAL(AHT10_OK, result);
}

TEST(Aht10Driver_TestGroup, Ah10Read_MeasSuccess) {
    uint8_t trigger_meas_cmd[] = {0xAC, 0x33, 0x00};
    // Sample values taken from a test program
    uint8_t fake_sensor_response[] = {0x1C, 0xBD, 0xC8, 0x66, 0x0E, 0xC2};
    aht10_measurement_t actual_measurement = {0};

    mock().expectOneCall("i2c_write")
        .withParameter("addr", AHT10_I2C_ADDR)
        .withMemoryBufferParameter("data", trigger_meas_cmd, sizeof(trigger_meas_cmd))
        .withParameter("len", 3)
        .andReturnValue(0);

    mock().expectOneCall("delay_ms")
          .withParameter("ms", AHT10_MEASUREMENT_DELAY_MS);

    mock().expectOneCall("i2c_read")
          .withParameter("addr", AHT10_I2C_ADDR)
          .withOutputParameterReturning("data", fake_sensor_response, sizeof(fake_sensor_response))
          .withParameter("len", 6)
          .andReturnValue(0);
    
    aht10_error_t result = aht10_read_measurement(&actual_measurement);

    LONGS_EQUAL(AHT10_OK, result);
    DOUBLES_EQUAL(25.72, actual_measurement.temperature_c, 0.1);
    DOUBLES_EQUAL(74.13, actual_measurement.humidity_rh, 0.1);

}

TEST(Aht10Driver_TestGroup, Ah10Read_Meas_SensorBusy) {
    uint8_t trigger_meas_cmd[] = {0xAC, 0x33, 0x00};
    // Sample values taken from a test program
    uint8_t fake_sensor_response[] = {0x8C, 0xBD, 0xC8, 0x66, 0x0E, 0xC2};
    aht10_measurement_t actual_measurement = {0};

    mock().expectOneCall("i2c_write")
        .withParameter("addr", AHT10_I2C_ADDR)
        .withMemoryBufferParameter("data", trigger_meas_cmd, sizeof(trigger_meas_cmd))
        .withParameter("len", 3)
        .andReturnValue(0);

    mock().expectOneCall("delay_ms")
          .withParameter("ms", AHT10_MEASUREMENT_DELAY_MS);

    mock().expectOneCall("i2c_read")
          .withParameter("addr", AHT10_I2C_ADDR)
          .withOutputParameterReturning("data", fake_sensor_response, sizeof(fake_sensor_response))
          .withParameter("len", 6)
          .andReturnValue(0);
    
    aht10_error_t result = aht10_read_measurement(&actual_measurement);

    LONGS_EQUAL(AHT10_ERR_SENSOR_BUSY, result);

}

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
