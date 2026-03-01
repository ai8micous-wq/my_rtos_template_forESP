\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_timer.h"

    static const char *TAG = "demo03";

    static void periodic_delay_task(void *arg)
    {
        (void)arg;
        while (1) {
            int64_t t = esp_timer_get_time();
            // 模拟“工作”耗时 20ms
            vTaskDelay(pdMS_TO_TICKS(20));
            ESP_LOGI(TAG, "[Delay]   now=%lld us (period target=100ms)", (long long)t);
            vTaskDelay(pdMS_TO_TICKS(100));  // 执行时间会叠加到周期里（抖动/漂移）
        }
    }

    static void periodic_delay_until_task(void *arg)
    {
        (void)arg;
        TickType_t last = xTaskGetTickCount();
        const TickType_t period = pdMS_TO_TICKS(100);

        while (1) {
            int64_t t = esp_timer_get_time();
            // 模拟“工作”耗时 20ms
            vTaskDelay(pdMS_TO_TICKS(20));
            ESP_LOGI(TAG, "[Until]   now=%lld us (period target=100ms)", (long long)t);
            vTaskDelayUntil(&last, period);  // 周期更稳定
        }
    }

    void demo_03_run(void)
    {
        ESP_LOGI(TAG, "DEMO 03 start: vTaskDelay vs vTaskDelayUntil");
        xTaskCreate(periodic_delay_task, "delay", 3072, NULL, 2, NULL);
        xTaskCreate(periodic_delay_until_task, "until", 3072, NULL, 2, NULL);
    }
