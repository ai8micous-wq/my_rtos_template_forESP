\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static const char *TAG = "demo05";
    static TaskHandle_t worker;

    static void notifier_task(void *arg)
    {
        (void)arg;
        uint32_t cnt = 0;
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(300));
            cnt++;
            // 作为“计数信号”：每次 +1
            xTaskNotifyGive(worker);
            ESP_LOGI(TAG, "notifier: give #%u", (unsigned)cnt);
        }
    }
    // 每当notifier任务运行一次xTaskNotifyGive(worker);，那么通知计数就会+1
    // 然后相应的任务（worker）就会马上被唤醒，进入Running状态。（如果没有被其他高优先级的任务抢占)

    static void worker_task(void *arg)
    {
        (void)arg;
        while (1) {
            // ulTaskNotifyTake 会把计数取出（可当二值/计数信号）
            uint32_t got = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            ESP_LOGI(TAG, "worker: got notify count=%u (do work 100ms)", (unsigned)got);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 这句话，按这两个参数的定义，如果没有被其他任务Notify，即通知计数为0
    // 那么worker任务会进入阻塞状态，一直等待，直到被Notify唤醒。（重点：一直阻塞！！！）
    // 其中ulTaskNotifyTake()返回，worker()处理任务过程中，被Notify的次数
    // 由于notifier任务每300ms通知一次，worker执行耗时才100ms，所以got的值一直都是1
    // 如果worker执行耗时大于notifier任务，那么got的值就会累积递增。
    //
    // 因此这个模型是一个非常轻量化的，任务间同步模型，适用于：
    // 1. 比 Queue/Semaphore 更轻量、更快、内存更少
    // 2. 非常适合 ISR 唤醒任务（例如 GPIO 中断、SPI DMA 完成中断）
    // 3. 常用来做“事件到来→唤醒任务处理”的模型
    // 例子：
    //     GPIO 中断来了 → 通知采样任务读取一次
    //     SPI DMA 完成 → 通知解析任务取 buffer
    //     WiFi 连接成功 → 通知上传任务开始跑（也可用 EventGroup）

    void demo_05_run(void)
    {
        ESP_LOGI(TAG, "DEMO 05 start: Task Notification (lightweight signaling)");
        xTaskCreate(worker_task, "worker", 3072, NULL, 4, &worker);
        // 第6个参数是任务句柄，用来保存创建任务成功后，读取存储它的句柄
        xTaskCreate(notifier_task, "notifier", 3072, NULL, 3, NULL);
    }
