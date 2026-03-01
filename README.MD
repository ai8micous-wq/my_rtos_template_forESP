# my_rtos_template_forESP

## 项目介绍（Introduction）
这是一个专为初学者打造的ESP-IDF项目模板，核心目标是帮助快速掌握：
- 如何从零创建一个标准的ESP-IDF工程结构
- FreeRTOS（ESP-IDF内置）的基础使用方法（任务创建、任务调度、队列/信号量等）
- ESP32/ESP32-C3等芯片的基础开发流程
- 项目包含最小化的可运行代码，无冗余功能，专注于核心知识点的学习和实践。
- FreeRTOS on ESP-IDF — 工业级框架模板 + 调度器原理 Demo（可直接编译运行）

本工程是一个 **ESP-IDF 单工程多 Demo** 的“工业级 FreeRTOS 框架模板”，以 **统一的组件化结构**组织多个 Demo，
方便你逐个启用、对照日志观察调度行为、阻塞/唤醒、优先级抢占、优先级反转与继承等机制。

## 支持的 Demo（通过 menuconfig 选择）
- DEMO 1：调度器/任务状态基础（Ready/Running/Blocked）
- DEMO 2：时间片与“饿死”现象（忙等 vs vTaskDelay）
- DEMO 3：vTaskDelayUntil 精准周期任务（抖动对比）
- DEMO 4：Queue 生产者-消费者（采样→上传）
- DEMO 5：Task Notification（轻量信号/计数）
- DEMO 6：EventGroup（系统状态同步：WIFI/MQTT/上传门控）
- DEMO 7：Mutex + 优先级反转/优先级继承实验
- DEMO 8：Software Timer（理解回调与 timer service task）
- DEMO 9：运行时诊断（堆/栈水位、任务列表、运行时间统计）

## 目录结构（工业级分层思路）
- `components/app_framework`：框架公共能力（日志、健康监控、系统事件、统计）
- `components/demos`：各 Demo 的实现
- `main/app_main.c`：只做“选择 Demo + 启动”

## 构建/烧录
```bash
idf.py set-target esp32c3
idf.py menuconfig
idf.py build flash monitor
```

## 使用建议
1. 先跑 DEMO 1/2/3，理解“阻塞式设计”的意义；
2. 再跑 DEMO 7，结合日志观察优先级反转与继承；
3. 最后把 DEMO 4/6 的结构迁移到你的真实项目：`sample_task → queue → upload_task`，用 EventGroup 门控联网状态。
