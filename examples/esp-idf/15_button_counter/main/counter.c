#include "counter.h"
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"

// Internal counter structure
struct button_counter {
    uint32_t count;
    lv_obj_t *label;
};

button_counter_t *button_counter_create(uint32_t initial_count)
{
    button_counter_t *counter = (button_counter_t *)calloc(1, sizeof(button_counter_t));
    if (counter != NULL) {
        counter->count = initial_count;
        counter->label = NULL;
    }
    return counter;
}

void button_counter_delete(button_counter_t *counter)
{
    if (counter != NULL) {
        free(counter);
    }
}

uint32_t button_counter_increment(button_counter_t *counter)
{
    if (counter == NULL) {
        return 0;
    }
    counter->count++;
    return counter->count;
}

uint32_t button_counter_get_count(button_counter_t *counter)
{
    if (counter == NULL) {
        return 0;
    }
    return counter->count;
}

void button_counter_set_label(button_counter_t *counter, void *label)
{
    if (counter != NULL) {
        counter->label = (lv_obj_t *)label;
    }
}

void button_counter_update_label(button_counter_t *counter)
{
    if (counter != NULL && counter->label != NULL) {
        lv_label_set_text_fmt(counter->label, "%lu", (unsigned long)counter->count);
    }
}
