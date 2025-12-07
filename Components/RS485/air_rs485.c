#include "air_rs485.h"
#include <stdio.h>
#include <errno.h>

// Helper to handle single register read, setting slave ID and providing detailed error logs
static int read_and_set_value(modbus_t *ctx, int slave_id, int reg_address, float divisor, float *value, const char* sensor_name) {
    modbus_set_slave(ctx, slave_id);
    uint16_t raw_value[1]; 

    if (modbus_read_registers(ctx, reg_address, 1, raw_value) == -1) {
        // Log the specific error details: Modbus error, register, slave ID, and system error message
        fprintf(stderr, "AIR_RS485 ERROR [%s, ID 0x%02X]: Failed to read register 0x%04X.\n", 
                  sensor_name, slave_id, reg_address);
        fprintf(stderr, " -> System Error: %s (errno: %d)\n", modbus_strerror(errno), errno);
        return AIR_RS485_E_SLAVE_READ;
    }
    
    // Apply scaling factor (no scaling if divisor is 0.0f)
    *value = (divisor != 0.0f) ? (float)raw_value[0] / divisor : (float)raw_value[0];
    return AIR_RS485_SUCCESS;
}


int air_rs485_read_epam(modbus_t *ctx, int slave_id, float *pm2_5_value, float *pm10_value) {
    int status;
    
    // Read PM2.5
    status = read_and_set_value(ctx, slave_id, EPAM_REG_PM25, 0.0f, pm2_5_value, "EPAM (PM2.5)");
    if (status != AIR_RS485_SUCCESS) return status;
    
    // Read PM10
    status = read_and_set_value(ctx, slave_id, EPAM_REG_PM10, 0.0f, pm10_value, "EPAM (PM10)");
    if (status != AIR_RS485_SUCCESS) return status;
    
    return AIR_RS485_SUCCESS;
}

int air_rs485_read_epco(modbus_t *ctx, int slave_id, float *co_value) {
    // Read CO concentration
    return read_and_set_value(ctx, slave_id, EPCO_REG_CO, EPCO_CO_DIVISOR, co_value, "EPCO (CO)");
}

int air_rs485_read_epno2(modbus_t *ctx, int slave_id, float *no2_value) {
    // Read NO2 concentration
    return read_and_set_value(ctx, slave_id, EPNO2_REG_NO2, EPNO2_NO2_DIVISOR, no2_value, "EPNO2 (NO2)");
}