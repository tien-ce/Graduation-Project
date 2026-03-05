#ifndef AIR_RS485_H
#define AIR_RS485_H

#include <stdint.h>
#include <modbus/modbus.h>
/**
 * @brief Initializes the Modbus RTU context.
 * @return Pointer to modbus_t context, or NULL on failure.
 */
modbus_t* rs485_init(const char* device, int baud, char parity, int data_bit, int stop_bit);

/**
 * @brief Reads a single raw 16-bit register from a specific slave.
 * @param ctx The Modbus context created in Python.
 * @param slave_id The ID of the physical sensor.
 * @param reg_addr The register address to read.
 * @param out_value Pointer to store the 16-bit result.
 * @return 0 on success, -1 on failure.
 */
int rs485_read_raw(modbus_t *ctx, int slave_id, int reg_addr, uint16_t *out_value);

/**
 * @brief Writes a single 16-bit value to a specific sensor register.
 * @param ctx The Modbus context.
 * @param slave_id The target slave ID.
 * @param reg_addr The register address to write to.
 * @param value The 16-bit value to write.
 * @return 0 on success, -1 on failure.
 */
int rs485_write_raw(modbus_t *ctx, int slave_id, int reg_addr, uint16_t value);

/**
 * @brief Closes the Modbus context and frees resources.
 * @param ctx The Modbus context to close.
 */
void rs485_close(modbus_t *ctx);
#endif
