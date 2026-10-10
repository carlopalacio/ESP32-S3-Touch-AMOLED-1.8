// Standard I/O, for printf-style output
#include <stdio.h>

// Managed BSP: board pins, display start, brightness, and LVGL lock helpers
#include "bsp/esp-bsp.h"
// ESP_LOGI / ESP_LOGE logging macros
#include "esp_log.h"
// FreeRTOS core types and macros (pdMS_TO_TICKS)
#include "freertos/FreeRTOS.h"
// FreeRTOS task API (vTaskDelay)
#include "freertos/task.h"
// LVGL graphics library: widgets, events, styles
#include "lvgl.h"
// Counter module for button press tracking
#include "counter.h"

// Tag used as the prefix for log messages from this file
static const char *TAG = "button_counter";

// Brightness used at startup, in percent
#define DEFAULT_BRIGHTNESS 85
// Number of counter buttons on the screen
#define BUTTON_COUNT 4

// One counter pointer per button, kept static so they live for the whole program
static button_counter_t *s_counters[BUTTON_COUNT];

// Runs every time any counter button is clicked; the counter to update arrives as user data
static void button_clicked_cb(lv_event_t *e)
{
    // Recover the counter pointer that was passed when the callback was registered
    button_counter_t *counter = (button_counter_t *)lv_event_get_user_data(e);
    // Increment the counter and update its label
    button_counter_increment(counter);
    button_counter_update_label(counter);
}

// Builds one colored button with a counter label inside it and returns the button
static lv_obj_t *create_counter_button(lv_obj_t *parent, lv_color_t color, lv_color_t text_color, button_counter_t *counter)
{
    // Create a button widget as a child of the parent container
    lv_obj_t *btn = lv_button_create(parent);
    // Make the button 160 px wide and 88 px tall so two fit per row
    lv_obj_set_size(btn, 160, 160);
    // Fill the button with the given color (style selector 0 = default state)
    lv_obj_set_style_bg_color(btn, color, 0);
    // Round the corners with a 24 px radius
    lv_obj_set_style_radius(btn, 24, 0);
    // Call button_clicked_cb on click and pass the counter pointer as its user data
    lv_obj_add_event_cb(btn, button_clicked_cb, LV_EVENT_CLICKED, counter);

    // Create the label that shows the count, as a child of the button
    lv_obj_t *label = lv_label_create(btn);
    // Register the label with the counter module
    button_counter_set_label(counter, label);
    // Use the 48 px Montserrat font so the number is large
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    // Set the text color, chosen so it reads on this button's background
    lv_obj_set_style_text_color(label, text_color, 0);
    // Initialize the label with the counter's current value
    button_counter_update_label(counter);
    // Center the label inside the button
    lv_obj_center(label);

    // Hand the button back to the caller
    return btn;
}

// Creates a transparent container that arranges its children in a wrapping grid
static lv_obj_t *create_button_grid(lv_obj_t *parent)
{
    // Create the container as a child of the parent screen
    lv_obj_t *grid = lv_obj_create(parent);
    // Make it 340 px wide, with the height sized to fit its children
    lv_obj_set_width(grid, 340);
    lv_obj_set_height(grid, LV_SIZE_CONTENT);
    // Make the container invisible: no background fill
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    // Remove the container's border so only the buttons are visible
    lv_obj_set_style_border_width(grid, 0, 0);
    // Remove the container's inner padding so the grid fills its box exactly
    lv_obj_set_style_pad_all(grid, 0, 0);
    // Keep the gap between columns at 12 px
    lv_obj_set_style_pad_column(grid, 12, 0);
    // Keep the gap between rows at 10 px
    lv_obj_set_style_pad_row(grid, 10, 0);
    // Place children left to right and wrap to a new row after two
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    // Center each row's buttons horizontally and center the rows vertically
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Hand the container back to the caller
    return grid;
}

// Entry point called by ESP-IDF after boot
void app_main(void)
{
    // Log that the app has started
    ESP_LOGI(TAG, "Starting button counter");

    // Initialize the display and touch input through the BSP and get the LVGL display handle
    lv_display_t *display = bsp_display_start();
    // If the display failed to start, there is nothing to draw on
    if (display == NULL) {
        // Log the failure
        ESP_LOGE(TAG, "display start failed");
        // Stop app_main; the program ends here
        return;
    }
    // Turn the backlight to the default brightness and abort if the BSP reports an error
    ESP_ERROR_CHECK(bsp_display_brightness_set(DEFAULT_BRIGHTNESS));

    // Take the LVGL lock (wait forever) so it is safe to create widgets from this task
    if (bsp_display_lock(0)) {
        // Get the default screen that all widgets will be placed on
        lv_obj_t *screen = lv_screen_active();
        // Stack child widgets in a vertical column
        lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
        // Center the children horizontally and vertically, and pack them in the middle
        lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Create the grid container that holds all the counter buttons
        lv_obj_t *grid = create_button_grid(screen);

        // Create counter instances for each button
        for (int i = 0; i < BUTTON_COUNT; i++) {
            s_counters[i] = button_counter_create(0);
        }

        // Create the yellow button, with black text for contrast
        create_counter_button(grid, lv_color_hex(0xFFEB3B), lv_color_black(), s_counters[0]);
        // Create the green button, with black text for contrast
        create_counter_button(grid, lv_color_hex(0x4CAF50), lv_color_black(), s_counters[1]);
        // Create the blue button, with white text for contrast
        create_counter_button(grid, lv_color_hex(0x2196F3), lv_color_white(), s_counters[2]);
        // Create the red button, with white text for contrast
        create_counter_button(grid, lv_color_hex(0xF44336), lv_color_white(), s_counters[3]);
        // Release the LVGL lock now that the UI is built
        bsp_display_unlock();
    } else {
        // The lock could not be taken, so the UI was not built
        ESP_LOGE(TAG, "failed to lock LVGL");
    }

    // Keep app_main alive; LVGL and the callbacks run in their own task
    while (true) {
        // Sleep for one second between iterations so this task doesn't spin the CPU
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
