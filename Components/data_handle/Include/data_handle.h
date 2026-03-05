#ifndef DATA_HANDLE_H
#define DATA_HANDLE_H

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//Error Logging Macro
#define LOG_ERR(msg, ...) fprintf(stderr, "[DATA HANDLE ERROR] " msg "\n", ##__VA_ARGS__)
#define LOG_INFO(msg, ...) fprintf(stdout, "[DATA HANDLE INFO] " msg "\n", ##__VA_ARGS__)

float calculate_median(float* values, int size);
float calculate_average(float* values, int size);

#endif