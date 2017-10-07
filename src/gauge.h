#pragma once

#include <stdbool.h>
#include <stdint.h>


void gauge_init();
uint16_t gauge_read_thermistor_b();
bool gauge_write_thermistor_b(uint16_t b);
float gauge_read_voltage();
