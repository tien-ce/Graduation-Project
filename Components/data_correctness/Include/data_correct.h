#include <stdbool.h>
#ifndef  __DATA_CORRECT
#define MAX_OB_SIZE 100 // Max window can be existed
// Type sensors
#define PM_SENSOR 0
#define CO_SENSOR 1
#define ERROR_LOG "Error from data_correct"
struct  ring_value {
	float value;
	struct ring_value* pre;
	struct ring_value* next;
};
struct ring_value_manager {
	int size;
	struct ring_value* head;
	struct ring_value* tail;

};
extern float calib_num;
extern int me_ob_size; // The window size for median filter
extern int calib_ob_size; // The window size for moving average			   

/**
 * @brief Calibrates the raw sensor data based on a specific calibration offset.
 * @param calib_num The offset value to apply to the sensor readings.
 * @return true if calibration was successful, false otherwise.
 */
bool calibrate_data (float calib_num);

/**
 * @brief Sets the window size (number of samples) used by the median filter.
 * @param me_ob_size The desired number of samples for the median calculation.
 * @return true if the size is within valid limits (0 to MAX_OB_SIZE), false otherwise.
 */
bool set_median_window_size (int me_ob_size);

/**
 * @brief Sets the window size used for the moving average calculation.
 * @param calib_ob_size The number of samples to average.
 * @return true if successfully set, false if the size exceeds MAX_OB_SIZE.
 */
bool set_calib_window_size (int calib_ob_size);

/* 
 * @brief return data after applying median_filter and moving_average 
 * @param new_data: Lastest data read from sensor
 * @return data after applied data_correct
*/
float data_correct (int sensor_type,float new_data);
#define __DATA_CORRECT
#endif
