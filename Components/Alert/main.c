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
    printf ("Open chip successfully");
    info = gpiod_chip_get_info(chip);
    if (!info) {
	fprintf(stderr, "failed to read info: %s\n", strerror(errno));
	abort();	
    }

    printf("%s [%s] (%zu lines)\n", gpiod_chip_info_get_name(info),
    gpiod_chip_info_get_label(info),
    gpiod_chip_info_get_num_lines(info));
    gpiod_chip_info_free(info);

    printf ("Get infor successfully");
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
void alert_init(uint8_t led, uint8_t buzzer){
    led_pin = led;
    buzzer_pin = buzzer;
    led_request = setup_pin(led_pin, "Alert-LED");
    buzzer_request = setup_pin(buzzer_pin, "Alert-Buzzer");

    if (!led_request || !buzzer_request) {
        fprintf(stderr, "%s Failed to reserve GPIO pins\n", ERROR_LOG);
        abort();
    }

    printf("Alert C initialized: LED (BCM %d) and Buzzer (BCM %d) ready.\n", 
            led_pin, buzzer_pin);
}

void alert_set_led(int state) {
    if (!led_request) return;

    enum gpiod_line_value value = state ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    gpiod_line_request_set_value(led_request, led_pin, value);
}

void alert_set_buzzer(int state) {
    if (!buzzer_request) return;

    enum gpiod_line_value value = state ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    gpiod_line_request_set_value(buzzer_request, buzzer_pin, value);
}

void alert_all_on(void) {
    alert_set_led(1);
    alert_set_buzzer(1);
}

int alert_get_led_state(void) {
    if (!led_request) return -1;
    return gpiod_line_request_get_value(led_request, led_pin);
}

int alert_get_buzzer_state(void) {
	if (!buzzer_request) return -1;
	return gpiod_line_request_get_value (buzzer_request, buzzer_pin);
}
