#include "Include/alert.h"
#include <stdio.h>
#include <stdlib.h>

// Definitions of the externs
struct gpiod_chip* chip = NULL;
struct gpiod_line_request* led_request = NULL;
struct gpiod_line_request* buzzer_request = NULL;

/*---------------------------- Private Function --------------------------------*/
static struct gpiod_line_request* setup_pin(unsigned int offset, const char* consumer) {
    struct gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    struct gpiod_line_config* line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);

    struct gpiod_request_config* req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, consumer);

    struct gpiod_line_request* request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    // Clean up temporary objects
    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);

    return request;
}

static void __attribute__((constructor)) _construction(void) {
    chip = gpiod_chip_open(CHIP_PATH);
    if (!chip) {
        fprintf(stderr, "%s Failed to open chip\n", ERROR_LOG);
        abort();
    }

    led_request = setup_pin(LED_PIN_OFFSET, "Alert-LED");
    buzzer_request = setup_pin(BUZZER_PIN_OFFSET, "Alert-Buzzer");

    if (!led_request || !buzzer_request) {
        fprintf(stderr, "%s Failed to reserve GPIO pins\n", ERROR_LOG);
        abort();
    }

    printf("Alert C initialized: LED (BCM %d) and Buzzer (BCM %d) ready.\n", 
            LED_PIN_OFFSET, BUZZER_PIN_OFFSET);
}

static void __attribute__((destructor)) _deconstruction(void) {
    // 1. Release the LED line request
    if (led_request != NULL) {
        gpiod_line_request_release(led_request);
        led_request = NULL;
    }

    // 2. Release the Buzzer line request
    if (buzzer_request != NULL) {
        gpiod_line_request_release(buzzer_request);
        buzzer_request = NULL;
    }

    // 3. Close the chip file descriptor
    if (chip != NULL) {
        gpiod_chip_close(chip);
        chip = NULL;
    }

    printf("Alert library resources released successfully.\n");
}


/*------------------------ Public Function -----------------------------*/
