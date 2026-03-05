#include "data_handle.h"
#include <stdlib.h>

// Comparison function for qsort
static int compare_floats(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

float calculate_median(float* values, int size) {
    if (size <= 0) {
        LOG_ERR("Invalid size (%d) for median calculation", size);
        return 0.0f;
    }

    // O(n log n) sorting using QuickSort
    qsort(values, size, sizeof(float), compare_floats);

    float result;
    if (size % 2 != 0) {
        result = values[size / 2];
    } else {    
        result = (values[(size - 1) / 2] + values[size / 2]) / 2.0f;
    }

    return result;
}

float calculate_average(float* values, int size) {
    if (size <= 0) {
        LOG_ERR("Invalid size (%d) for average calculation", size);
        return 0.0f;
    }

    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += values[i];
    }
    
    return sum / (float)size;
}