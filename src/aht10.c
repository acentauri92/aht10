#include <stdio.h>
#include "aht10.h"
#include "i2c_hal.h"

aht10_error_t ah10_init(void) {

    const uint8_t soft_reset_command = AHT10_SOFT_RESET_CMD;
    const uint8_t init_command[3] = {0xE1, 0x08, 0x00};

    aht10_error_t result = i2c_write(AHT10_I2C_ADDR, &soft_reset_command, 
                                sizeof(soft_reset_command));
    if(result != 0) {
        fprintf(stderr, "AHT10 ERROR: Failed to send soft reset command.\n");
        return AHT10_ERR_I2C_WRITE;
    }
        
    delay_ms(AHT10_SOFT_RESET_DELAY_MS);

    result = i2c_write(AHT10_I2C_ADDR, init_command, sizeof(init_command));
    if(result != 0) {
        fprintf(stderr, "AHT10 ERROR: Failed to send initialization command.\n");
        return AHT10_ERR_I2C_WRITE;
    }
        
    return AHT10_OK;
}

aht10_error_t aht10_read_measurement(aht10_measurement_t* measurement) {
    const uint8_t trigger_meas_cmd[] = {0xAC, 0x33, 0x00};
    uint8_t response[6];

    aht10_error_t result = i2c_write(AHT10_I2C_ADDR, trigger_meas_cmd,
                            sizeof(trigger_meas_cmd));
    if(result != 0) {
        fprintf(stderr, "AHT10 ERROR: Failed to send trigger measurement command.\n");
        return AHT10_ERR_I2C_WRITE;
    }

    delay_ms(AHT10_MEASUREMENT_DELAY_MS);

    result = i2c_read(AHT10_I2C_ADDR, response, sizeof(response));
    if(result != 0) {
        fprintf(stderr, "AHT10 ERROR: Failed to read response.\n");
        return AHT10_ERR_I2C_READ;
    }

    // First byte returned is the status byte. Section 5.3 of satasheet
    if ((response[0] & 0x80) != 0) {
        fprintf(stderr, "AHT10 Error: Sensor is busy.\n");
        return AHT10_ERR_SENSOR_BUSY;
    }

    // First 20 bits are of humidity
    uint32_t raw_humidity = ((uint32_t)response[1] << 12) | 
                            ((uint32_t)response[2] << 4) | 
                            (response[3] >> 4);

    uint32_t raw_temperature = ( (uint32_t)(response[3] & 0x0F) << 16 ) | 
                                ((uint32_t)(response[4] << 8)) | 
                                response[5];

    measurement->humidity_rh = ((float)raw_humidity / 1048576.0f) * 100.0f;
    measurement->temperature_c = (((float)raw_temperature / 1048576.0f) * 200.0f) - 50.0f;

    return AHT10_OK;
}