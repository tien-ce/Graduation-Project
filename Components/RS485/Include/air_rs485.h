#ifndef AIR_RS485_H
#define AIR_RS485_H

#include <stdint.h>
#include <modbus/modbus.h>

// --- Register Addresses and Scaling ---
#define EPAM_REG_PM25   0x0004
#define EPAM_REG_PM10   0x0009

#define EPCO_REG_CO     0x0006
#define EPCO_CO_DIVISOR 100.0f

#define EPNO2_REG_NO2   0x0006
#define EPNO2_NO2_DIVISOR 100.0f

// --- Error Codes (Log messages will provide details) ---
#define AIR_RS485_SUCCESS       0
#define AIR_RS485_E_SLAVE_READ  -3 // Failed to read registers

/**
 * @brief Reads PM2.5 and PM10 values from the EPAM sensor.
 * @param ctx The initialized Modbus context.
 * @param slave_id The Modbus slave ID of the sensor.
 * @param pm2_5_value Pointer to store the PM2.5 concentration (float).
 * @param pm10_value Pointer to store the PM10 concentration (float).
 * @return AIR_RS485_SUCCESS (0) on success, or a negative error code on failure.
 */
int air_rs485_read_epam(modbus_t *ctx, int slave_id, float *pm2_5_value, float *pm10_value);

/**
 * @brief Reads the CO concentration value from the EPCO sensor.
 * @param ctx The initialized Modbus context.
 * @param slave_id The Modbus slave ID of the sensor.
 * @param co_value Pointer to store the CO concentration (float, scaled by 100).
 * @return AIR_RS485_SUCCESS (0) on success, or a negative error code on failure.
 */
int air_rs485_read_epco(modbus_t *ctx, int slave_id, float *co_value);

/**
 * @brief Reads the NO2 concentration value from the EPNO2 sensor.
 * @param ctx The initialized Modbus context.
 * @param slave_id The Modbus slave ID of the sensor.
 * @param no2_value Pointer to store the NO2 concentration (float, scaled by 100).
 * @return AIR_RS485_SUCCESS (0) on success, or a negative error code on failure.
 */
int air_rs485_read_epno2(modbus_t *ctx, int slave_id, float *no2_value);

#endif // AIR_RS485_H