# 15_button_counter

[English](README.md)

使用 LVGL 绘制的两个屏幕触摸按钮：一个蓝色，一个红色。每个按钮上的数字统计该按钮被按下的次数，两个计数互不影响。

## 硬件

- ESP32-S3-Touch-AMOLED-1.8 开发板
- USB 数据线

## 构建和烧录

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

将 `PORT` 替换为开发板的串口。

## 预期结果

屏幕上方为蓝色按钮，下方为红色按钮，两者初始显示 `0`。每次点击只会增加被点击按钮的计数。

## 注意事项

- BSP 负责启动显示屏和触摸输入；界面在 `bsp_display_lock()` 保护下创建。
- 按钮回调在 LVGL 任务中执行，因此计数器无需额外加锁。
