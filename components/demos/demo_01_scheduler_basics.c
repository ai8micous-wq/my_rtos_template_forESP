\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static const char *TAG = "demo01";

    static void task_a(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "A: Running -> delay 1000ms (enter Blocked)");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    static void task_b(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "B: Running -> delay 500ms (enter Blocked)");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    // 任务C：短忙等 + 让出 CPU（对比“完全不让出”）
    static void task_c(void *arg)
    {
        (void)arg;
        while (1) {
            // 做一点“工作”
            for (volatile int i = 0; i < 200000; i++) { }
            // 让出一次 CPU，避免把低优先级任务饿死（也便于观察调度）
            taskYIELD();
        }
    }

    void demo_01_run(void)
    {
        ESP_LOGI(TAG, "DEMO 01 start: Scheduler basics (task states)");
        xTaskCreate(task_a, "A_1s", 2048, NULL, 2, NULL);
        xTaskCreate(task_b, "B_500ms", 2048, NULL, 3, NULL);
        xTaskCreate(task_c, "C_yield", 2048, NULL, 1, NULL);
    }
