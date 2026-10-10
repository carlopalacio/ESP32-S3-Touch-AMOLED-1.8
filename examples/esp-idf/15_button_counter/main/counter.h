#pragma once

#include <stdint.h>

// Opaque forward declaration — implementation hides LVGL dependency
typedef struct button_counter button_counter_t;

// Create a new counter instance with the given initial value
button_counter_t *button_counter_create(uint32_t initial_count);

// Free a counter instance
void button_counter_delete(button_counter_t *counter);

// Increment the counter and return the new value
uint32_t button_counter_increment(button_counter_t *counter);

// Get the current count value
uint32_t button_counter_get_count(button_counter_t *counter);

// Set the display label for this counter (call after creating the LVGL label)
void button_counter_set_label(button_counter_t *counter, void *label);

// Update the label text to reflect the current count (call from button callback)
void button_counter_update_label(button_counter_t *counter);
