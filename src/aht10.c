#include <stdio.h>
#include "aht10.h"
#include "i2c_hal.h"

aht10_error_t ah10_init(void) {

    const uint8_t soft_reset_command = AHT10_SOFT_RESET_CMD;
    const uint8_t init_command[3] = {0xE1, 0x08, 0x00};

    int result = i2c_write(AHT10_I2C_ADDR, &soft_reset_command, sizeof(soft_reset_command));
    if(result != 0) {
        fprintf(stderr, "AHT10 Error: Failed to send soft reset command.\n");
        return AHT10_ERR_I2C_WRITE;
    }
        
    
    delay_ms(AHT10_SOFT_RESET_DELAY_MS);

    result = i2c_write(AHT10_I2C_ADDR, init_command, sizeof(init_command));
    if(result != 0) {
        fprintf(stderr, "AHT10 Error: Failed to send initialization command.\n");
        return AHT10_ERR_I2C_WRITE;
    }
        

    return 0;
}