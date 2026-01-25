#ifndef PM_SENSOR_H
#define PM_SENSOR_H

#include <modbus/modbus.h>
#include <stdint.h>

// Register offsets from datasheet
#define PM_REG_PM25 0x0004
#define PM_REG_PM10 0x0009
#define PM_REG_BAUD 0x0101
#define PM_REG_ADDR 0x0100

typedef struct {
    uint8_t slave_addr;
    uint32_t baudrate;
    char device_path[32]; // e.g., "/dev/ttyUSB0"
    modbus_t *ctx;
} PMSensor_t;

// Lifecycle and Config
int pm_sensor_init(PMSensor_t *s, const char *config_file);
int pm_sensor_save_config(PMSensor_t *s, const char *config_file);

// Data Operations
int pm_sensor_read_data(PMSensor_t *s, float *pm25, float *pm10);
int pm_sensor_check_connection(PMSensor_t *s);

// Hardware Configuration (Writes to Device and local struct)
int pm_sensor_update_baud(PMSensor_t *s, uint32_t new_baud);

#endif
