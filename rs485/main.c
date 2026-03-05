#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <modbus/modbus.h>

#define SLAVE_ID   0x24
#define REG_PM25   0x0004
#define REG_PM10   0x0009

int main(void) {
    int baudrate = 9600;

    // --- Initialize Modbus RTU over USB-RS485 ---
    modbus_t *ctx = modbus_new_rtu("/dev/ttyUSB0", baudrate, 'N', 8, 1);
    if (!ctx) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    printf("Modbus RTU context created for /dev/ttyUSB0 at %d baud\n", baudrate);

    modbus_set_slave(ctx, SLAVE_ID);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Unable to connect to Modbus slave\n");
        modbus_free(ctx);
        return -1;
    }
    printf("Connected to Modbus slave ID %d, baud rate %d\n", SLAVE_ID, baudrate);

    uint16_t tab_reg[2];

    while (1) {
        if (modbus_read_registers(ctx, REG_PM25, 1, &tab_reg[0]) == -1) {
            fprintf(stderr, "Failed to read PM2.5\n");
        } else {
            printf("PM2.5: %.2f\n", (float)tab_reg[0]);
        }

        if (modbus_read_registers(ctx, REG_PM10, 1, &tab_reg[1]) == -1) {
            fprintf(stderr, "Failed to read PM10\n");
        } else {
            printf("PM10: %.2f\n", (float)tab_reg[1]);
        }

        sleep(1);
    }

    // --- Clean up Modbus ---
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
