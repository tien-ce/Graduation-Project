#include "air_rs485.h"
#include <stdio.h>
#include <errno.h>

int rs485_read_raw(modbus_t *ctx, int slave_id, int reg_addr, uint16_t *out_value) {
    if (ctx == NULL) return -1;

    // Set the target slave for this specific transaction 
    if (modbus_set_slave(ctx, slave_id) == -1) {
        fprintf(stderr, "RS485: Failed to set slave ID 0x%02X\n", slave_id);
        return -1;
    }

    uint16_t buffer[1];
    // Perform the physical read 
    if (modbus_read_registers(ctx, reg_addr, 1, buffer) == -1) {
        fprintf(stderr, "RS485 Error: Slave 0x%02X, Reg 0x%04X - %s\n", 
                slave_id, reg_addr, modbus_strerror(errno));
        return -1;
    }

    *out_value = buffer[0];
    return 0;
}

int rs485_write_raw(modbus_t *ctx, int slave_id, int reg_addr, uint16_t value) {
    if (ctx == NULL) return -1;

    // Target the specific sensor
    if (modbus_set_slave(ctx, slave_id) == -1) {
        fprintf(stderr, "RS485 WRITE ERROR: Invalid slave ID 0x%02X\n", slave_id);
        return -1;
    }

    // Perform the write operation
    if (modbus_write_register(ctx, reg_addr, value) == -1) {
        fprintf(stderr, "RS485 WRITE ERROR [ID 0x%02X, Reg 0x%04X]: %s\n", 
                slave_id, reg_addr, modbus_strerror(errno));
        return -1;
    }

    return 0;
}
