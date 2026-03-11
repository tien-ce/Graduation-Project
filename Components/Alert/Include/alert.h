#ifndef _ALERT_H
#define _ALERT_H

#include <stdint.h>
#include <gpiod.h>

// Use macros for constants to avoid memory allocation in header
#define CHIP_PATH "/dev/gpiochip0"
#define ERROR_LOG "[Error at alert library]"

uint8_t led_pin = -1;
uint8_t buzzer_pin = -1;
// Extern declarations (Defined in alert.c)
extern struct gpiod_chip* chip;
extern struct gpiod_line_request* led_request;
extern struct gpiod_line_request* buzzer_request;

/*-------------------- Function declearation---------------------*/

void alert_init (uint8_t led, uint8_t buzzer);
/**
 * @brief Sets the state of the Alert LED.
 * * @param state 1 to turn the LED on (High), 0 to turn it off (Low).
 */
void alert_set_led(int state);

/**
 * @brief Sets the state of the Alert Buzzer.
 * * @param state 1 to activate the buzzer, 0 to deactivate.
 */
void alert_set_buzzer(int state);

/**
 * @brief Checks the current logical state of the LED line.
 * * @return 1 if High, 0 if Low, or -1 if the line is not initialized.
 */
int alert_get_led_state(void);

/**
 * @brief Checks the current logical state of the BUZZER line.
 * * @return 1 if High, 0 if Low, or -1 if the line is not initialized.
 */
int alert_get_buzzer_state(void);


/**
 * @brief Immediately activates both the LED and the Buzzer.
 */
void alert_all_on(void);

/**
 * @brief Immediately deactivates both the LED and the Buzzer.
 */
void alert_all_off(void);
#endif
