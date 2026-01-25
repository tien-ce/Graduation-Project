#include "pm_sensor.h"
#include <stdio.h>

int pm_sensor_init(PMSensor_t *s, const char *config_file) {
    // 1. Load from disk
    FILE *f = fopen(config_file, "r");
    if (f) {
        fscanf(f, "ADDR=%hhx\nBAUD=%u\nPATH=%s", &s->slave_addr, &s->baudrate, s->device_path);
        fclose(f);
    } else {
        // Fallback to defaults 
        s->slave_addr = 0x24;
        s->baudrate = 4800;
        sprintf(s->device_path, "/dev/ttyUSB0");
    }

    // 2. Setup libmodbus context
    s->ctx = modbus_new_rtu(s->device_path, s->baudrate, 'N', 8, 1);
    if (s->ctx == NULL) return -1;

    modbus_set_slave(s->ctx, s->slave_addr);
    
    if (modbus_connect(s->ctx) == -1) {
        modbus_free(s->ctx);
        return -1;
    }
    return 0;
}

int pm_sensor_read_data(PMSensor_t *s, float *pm25, float *pm10) {
    uint16_t tab_reg[10]; // Buffer for holding registers

    // Read PM2.5 (0x0004)
    if (modbus_read_registers(s->ctx, PM_REG_PM25, 1, tab_reg) > 0) {
        *pm25 = (float)tab_reg[0];
    } else return -1;

    // Read PM10 (0x0009)
    if (modbus_read_registers(s->ctx, PM_REG_PM10, 1, tab_reg) > 0) {
        *pm10 = (float)tab_reg[0];
        return 0; // Success
    }
    
    return -1; // Device communication error
}

int pm_sensor_check_connection(PMSensor_t *s) {
    uint16_t dummy;
    // Try reading the address register to verify presence
    return (modbus_read_registers(s->ctx, PM_REG_ADDR, 1, &dummy) > 0);
}

int pm_sensor_update_baud(PMSensor_t *s, uint32_t new_baud) {
    // Write to sensor register 0x0101
    // Note: Actual Modbus value usually maps 0=2400, 1=4800, 2=9600
    uint16_t baud_code = (new_baud == 9600) ? 2 : (new_baud == 4800) ? 1 : 0;
    
    if (modbus_write_register(s->ctx, PM_REG_BAUD, baud_code) != -1) {
        s->baudrate = new_baud;
        return 0;
    }
    return -1;
}

int pm_sensor_save_config(PMSensor_t *s, const char *config_file) {
    FILE *f = fopen(config_file, "w");
    if (!f) return -1;
    fprintf(f, "ADDR=%02x\nBAUD=%u\nPATH=%s", s->slave_addr, s->baudrate, s->device_path);
    fclose(f);
    return 0;
}
