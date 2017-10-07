#pragma once

#include <stdbool.h>
#include <stdint.h>


typedef enum {
    HALL_1,
    // HALL_2,
    HALL_3
} hall_sensor_t;

void hall_init();
void hall_set_sleep(hall_sensor_t sensor, bool sleep);
uint16_t hall_sample(hall_sensor_t sensor);
