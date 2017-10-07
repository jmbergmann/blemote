#pragma once

#include <stdbool.h>
#include <stdint.h>


void twi_init();
bool twi_read(uint8_t address, uint8_t* buffer, uint8_t size);
bool twi_write(uint8_t address, const uint8_t* buffer, uint8_t size);
bool twi_write_no_stop(uint8_t address, const uint8_t* buffer, uint8_t size);
