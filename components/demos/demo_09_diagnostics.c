\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "app_framework.h"

    static const char *TAG = "demo09";

    static void diag_task(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "=== Diagnostics snapshot ===");
            app_health_print_once();
            app_stats_print_task_list_once();
            app_stats_print_runtime_once();
            vTaskDelay(pdMS_TO_TICKS(7000));
        }
    }

    static void dummy_load(void *arg)
    {
        (void)arg;
        while (1) {
            // 做点负载，便于 runtime stats 有差异
            for (volatile int i = 0; i < 200000; i++) { }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    void demo_09_run(void)
    {
        ESP_LOGI(TAG, "DEMO 09 start: Diagnostics (task list/runtime stats)");
        xTaskCreate(diag_task, "diag", 4096, NULL, 2, NULL);
        xTaskCreate(dummy_load, "load1", 2048, NULL, 3, NULL);
        xTaskCreate(dummy_load, "load2", 2048, NULL, 3, NULL);
    }
