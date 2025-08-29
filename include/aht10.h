#ifndef AHT10_H
#define AHT10_H

#include <stdint.h>

#define AHT10_I2C_ADDR                  0x38

#define AHT10_SOFT_RESET_DELAY_MS       20U
#define AHT10_MEASUREMENT_DELAY_MS      75U

#define AHT10_SOFT_RESET_CMD            0xBA
#define AHT10_INIT_CMD                  0xE10800


typedef enum {
    AHT10_OK = 0,
    AHT10_ERR_I2C_WRITE = -1,
    AHT10_ERR_I2C_READ  = -2,
    AHT10_ERR_SENSOR_BUSY
} aht10_error_t;

typedef struct {
    float temperature_c;
    float humidity_rh;
} aht10_measurement_t;

/**
 * @brief Initializes the AHT10 sensor.
 * @return AHT10_OK on success, error code on failure.
 */
aht10_error_t ah10_init(void);

/**
 * @brief Trigger and read measurement from the aht10 sensor
 * @param measurement Pointer to a struct to store the result
 * @return AHT10_OK on success, error code on failure.
 */
aht10_error_t aht10_read_measurement(aht10_measurement_t* measurement);

#endif //AHT10_H