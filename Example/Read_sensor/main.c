#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "air_rs485.h" // Our new library

// --- Configuration: Slave IDs ---
#define EPAM_SLAVE_ID_CFG   0x24  // Default ID for PM2.5/PM10 sensor
#define EPCO_SLAVE_ID_CFG   0x25  // Default ID for CO sensor
#define EPNO2_SLAVE_ID_CFG  0x23  // Default ID for NO2 sensor

#define BAUD_RATE 9600
#define DEVICE_PORT "/dev/ttyUSB0"

// Helper function to handle connection errors
modbus_t* setup_modbus_connection() {
    modbus_t *ctx = modbus_new_rtu(DEVICE_PORT, BAUD_RATE, 'N', 8, 1);
    if (!ctx) {
        fprintf(stderr, "FATAL ERROR: Unable to allocate libmodbus context.\n");
        return NULL;
    }
    
    // You can set timeouts here if needed
    // modbus_set_response_timeout(ctx, 0, 500000); // e.g., 500ms timeout

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "FATAL ERROR: Unable to connect to Modbus port %s.\n", DEVICE_PORT);
        fprintf(stderr, " -> System Error: %s (errno: %d)\n", modbus_strerror(errno), errno);
        modbus_free(ctx);
        return NULL;
    }
    printf("Connection successful on %s at %d baud.\n", DEVICE_PORT, BAUD_RATE);
    return ctx;
}

int main(void) {
    modbus_t *ctx = setup_modbus_connection();
    if (!ctx) {
        return EXIT_FAILURE;
    }

    float pm2_5, pm10, co_value, no2_value;

    while (1) {
        printf("\n--- Reading Cycle ---\n");
        int status;

        // 1. Read EPAM Sensor (PM2.5/PM10)
        status = air_rs485_read_epam(ctx, EPAM_SLAVE_ID_CFG, &pm2_5, &pm10);
        if (status == AIR_RS485_SUCCESS) {
            printf("[EPAM - 0x%02X] PM2.5: %.2f µg/m³, PM10: %.2f µg/m³\n", EPAM_SLAVE_ID_CFG, pm2_5, pm10);
        } // Error details logged by the library

        // 2. Read EPCO Sensor (CO)
        status = air_rs485_read_epco(ctx, EPCO_SLAVE_ID_CFG, &co_value);
        if (status == AIR_RS485_SUCCESS) {
            printf("[EPCO - 0x%02X] CO: %.2f ppm\n", EPCO_SLAVE_ID_CFG, co_value);
        } // Error details logged by the library

        // 3. Read EPNO2 Sensor (NO2)
        status = air_rs485_read_epno2(ctx, EPNO2_SLAVE_ID_CFG, &no2_value);
        if (status == AIR_RS485_SUCCESS) {
            printf("[EPNO2 - 0x%02X] NO2: %.2f ppm\n", EPNO2_SLAVE_ID_CFG, no2_value);
        } // Error details logged by the library

        sleep(1);
    }

    // --- Clean up Modbus ---
    modbus_close(ctx);
    modbus_free(ctx);

    return EXIT_SUCCESS;
}