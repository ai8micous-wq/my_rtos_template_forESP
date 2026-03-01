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

    void demo_05_run(void)
    {
        ESP_LOGI(TAG, "DEMO 05 start: Task Notification (lightweight signaling)");
        xTaskCreate(worker_task, "worker", 3072, NULL, 4, &worker);
        xTaskCreate(notifier_task, "notifier", 3072, NULL, 3, NULL);
    }
