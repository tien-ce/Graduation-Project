#include <stdio.h>
#include <stdlib.h>
#include "data_correct.h"
void add_value(struct ring_value_manager* sensor ,float new_val) {
        if (sensor->size < MAX_OB_SIZE) {
            // Case 1: Still growing - allocate new memory
            struct ring_value* new_node = malloc(sizeof(struct ring_value));
            new_node->value = new_val;
            new_node->next = NULL;

            if (sensor->head == NULL) {
                sensor->head = sensor->tail = new_node;
                new_node->pre = NULL;
            } else {
                sensor->tail->next = new_node;
                new_node->pre = sensor->tail;
                sensor->tail = new_node;
            }
            sensor->size++;
        } else {
            // Case 2: Buffer is full - rotate the head to the tail
            // The old head contains the oldest data; reuse it for the newest data
            sensor->head->value = new_val;

            // Move the head pointer to the second node
            struct ring_value* old_head = sensor->head;
            sensor->head = old_head->next;
            sensor->head->pre = NULL;

            // Move old_head to the end of the list
            sensor->tail->next = old_head;
            old_head->pre = sensor->tail;
            old_head->next = NULL;

            // Update tail to be the newly moved node
            sensor->tail = old_head;
        }
}
struct ring_value_manager pm_sensor; 
struct ring_value_manager co_sensor;

// Define the default values
float calib_num = 0;
int me_ob_size = 3;
int calib_ob_size = 6;

// Function use in filter
/* 
 * @brief 
 *
*/
static float find_median (float* values, int size){
	if (size <= 0) return 0.0f;
	
    	// 1. Sort the array (Simple Bubble Sort for small windows)
    	for (int i = 0; i < size - 1; i++) {
        	for (int j = 0; j < size - i - 1; j++) {
            		if (values[j] > values[j + 1]) {
                		float temp = values[j];
                		values[j] = values[j + 1];
                		values[j + 1] = temp;
            		}
        	}
    	}

   	 // 2. Pick the middle value
   	 if (size % 2 != 0) {
        	// Odd size: return the exact middle
       		 return values[size / 2];
    	} else {
        	// Even size: return the average of the two middle values
        	return (values[(size - 1) / 2] + values[size / 2]) / 2.0f;
    	}
	
}


/* 
 * @brief 
 *
*/
static bool isAbnormalData (float data){
	return false;
}


/* 
 * @brief 
 *
*/
static float median_filter (float new_data, int type){
	struct ring_value_manager* sensor = NULL;	
	switch (type){
		case PM_SENSOR:
			sensor = &pm_sensor;
			break;
		case CO_SENSOR:
			sensor = &co_sensor;
			break;
		default:
			printf ("%s: Sensor type error\n",ERROR_LOG);
			return -1.0f;
	}
	// Always add the data to the history buffer first
	add_value(sensor,new_data);
	if (sensor->size < 2) {
		return new_data;
	}
	else {
		// Determine window size
    		int ob_size = (me_ob_size <= sensor->size) ? me_ob_size : sensor->size;
    		float values[MAX_OB_SIZE];

    		struct ring_value* temp = sensor->tail;
    		for (int i = 0; i < ob_size && temp != NULL; i++) {
        		values[i] = temp->value;
        		temp = temp->pre; // Moving backward through the doubly linked list
  		  }

    		return find_median(values, ob_size); // Pass the values to be sorted and picked		
	}
}

/**
 * @brief Calculates the moving average of the sensor data to smooth out fluctuations.
 * It retrieves the last N values (defined by calib_ob_size) from the ring_value_manager
 * and returns their arithmetic mean.
 * @param new_data The filtered data point to be added to the average.
 * @return The calculated moving average value.
 */
float moving_average(float new_data, int type){
	struct ring_value_manager* sensor;
	switch (type){
		case PM_SENSOR:
			sensor = &pm_sensor;
			break;
		case CO_SENSOR:
			sensor = &co_sensor;
			break;
		default:
			printf ("%s: Sensor type error\n",ERROR_LOG);
			return -1.0f;
	}
	if (sensor == NULL || sensor->head == NULL || calib_ob_size <= 0) {
		return -1.0f;
	}

	float sum = 0.0f;
	int count = 0;
	struct ring_value* current = sensor->tail;

	// Traverse backwards to sum the most recent 'calib_ob_size' values
	while (current != NULL && count < calib_ob_size) {
		sum += current->value;
		current = current->pre;
		count++;
	}

	return sum / (float)count;
}

// Implement the export functions
/**
 * @brief Calibrates the data by applying the offset and handling abnormal cases.
 */
bool calibrate_data(float calib_num_input) {
	// Basic implementation: update global calibration value
	calib_num = calib_num_input;

	// Logic for isAbnormalData will be implemented here later as requested.
	return true;
}

/**
 * @brief Sets the median window size within valid bounds.
 */
bool set_median_window_size(int size) {
	if (size > 0 && size <= MAX_OB_SIZE) {
		me_ob_size = size;
		return true;
	}
	printf ("%s: median size must be in 0 < %d",ERROR_LOG,MAX_OB_SIZE);
	return false;
}

/**
 * @brief Sets the moving average window size within valid bounds.
 */
bool set_calib_window_size(int size) {
	if (size > 0 && size <= MAX_OB_SIZE) {
		calib_ob_size = size;
		return true;
	}
	printf ("%s: median size must be in 0 < %d",ERROR_LOG,MAX_OB_SIZE);
	return false;
}

/**
  * @brief Calculate data affter correctness
*/

float data_correct(int sensor_type,float new_data){
	float medianed_value = median_filter(new_data,sensor_type);
	if (medianed_value == -1.0f)
		return -1.0f;
	float moving_averaged_value = moving_average(new_data,sensor_type);
	return moving_averaged_value;
}


// Function when lib is loaded and unloaded 


/**
 * @brief Frees all dynamically allocated nodes in a sensor manager.
 */
void sensor_manager_cleanup(struct ring_value_manager* sensor) {
    if (sensor == NULL || sensor->head == NULL) return;

    struct ring_value* current = sensor->head;
    while (current != NULL) {
        struct ring_value* next_node = current->next;
        free(current);
        current = next_node;
    }

    sensor->head = NULL;
    sensor->tail = NULL;
    sensor->size = 0;
}



/**
 * @brief Automatically initializes sensor managers upon library load.
 * * This function is executed by the dynamic linker as soon as the library 
 * is mapped into the process memory space.
 */
__attribute__((constructor))
static void library_load_init() {
    // Initialize PM Sensor
    pm_sensor.head = NULL;
    pm_sensor.tail = NULL;
    pm_sensor.size = 0;   

    // Initialize CO Sensor
    co_sensor.head = NULL;
    co_sensor.tail = NULL;
    co_sensor.size = 0;   

    // Optional: Log initialization for debugging
    // printf("%s: Sensor managers initialized.\n", ERROR_LOG);
}

// This function runs automatically when the library is unloaded
__attribute__((destructor))
static void library_unload_cleanup() {
    sensor_manager_cleanup(&pm_sensor);
    sensor_manager_cleanup(&co_sensor);
    printf("%s: Library memory cleaned up.\n", ERROR_LOG);
}

