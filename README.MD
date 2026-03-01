# my_rtos_template_forESP

## 项目介绍（Introduction）
这是一个专为初学者打造的ESP-IDF项目模板，核心目标是帮助快速掌握：
- 如何从零创建一个标准的ESP-IDF工程结构
- FreeRTOS（ESP-IDF内置）的基础使用方法（任务创建、任务调度、队列/信号量等）
- ESP32/ESP32-C3等芯片的基础开发流程

项目包含最小化的可运行代码，无冗余功能，专注于核心知识点的学习和实践。

## 环境准备（Environment Setup）
### 前置依赖
1. 安装ESP-IDF开发环境（推荐v5.0+）：
   - 官方教程：[ESP-IDF 快速入门](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)
   - 支持的操作系统：Windows/macOS/Linux
2. 支持的芯片：ESP32、ESP32-C3、ESP32-S3（代码已做兼容性适配）
3. 硬件：ESP32开发板（任意款）、USB数据线

### 环境验证
```bash
# 检查ESP-IDF是否安装成功
idf.py --version

# 检查串口（确认开发板已连接）
# Windows: 设备管理器查看COM口
# Linux/macOS: ls /dev/tty* (通常为/dev/ttyUSB0或/dev/tty.usbserial-*)
