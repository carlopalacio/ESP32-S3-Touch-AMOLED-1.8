# 15_button_counter

[简体中文](README_ZH.md)

Two on-screen touch buttons drawn with LVGL: a blue one and a red one. Each button shows a number that counts how many times it has been pressed. The two counters are independent.

## Hardware

- ESP32-S3-Touch-AMOLED-1.8 board
- USB data cable

## Build and Flash

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the serial port for the board.

## Expected Result

The screen shows a blue button on top of a red button, both starting at `0`. Each tap increments only the counter of the button that was tapped.

## Notes

- The BSP starts the display and touch input; the UI is built under `bsp_display_lock()`.
- Button callbacks run in the LVGL task, so the counters need no extra locking.
