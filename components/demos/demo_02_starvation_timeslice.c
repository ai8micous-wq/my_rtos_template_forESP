\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static const char *TAG = "demo02";

    static void polite_task(void *arg)
    {
        const char *name = (const char *)arg;
        while (1) {
            ESP_LOGI(TAG, "%s: prints then vTaskDelay(200ms)", name);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }

    static void hog_task(void *arg)
    {
        (void)arg;
        ESP_LOGW(TAG, "HOG: starts infinite loop WITHOUT delay. Observe starvation.");
        while (1) {
            // 模拟“业务忙等”（非常不推荐）
            for (volatile int i = 0; i < 800000; i++) { }
            // 注意：此处故意不 delay；如果同优先级、并开启 time slicing，才会轮转
        }
    }

    void demo_02_run(void)
    {
        ESP_LOGI(TAG, "DEMO 02 start: Starvation & timeslice");
        // 两个同优先级任务，看看 time-slicing 影响（取决于配置）
        xTaskCreate(polite_task, "polite1", 2048, "polite1", 2, NULL);
        xTaskCreate(polite_task, "polite2", 2048, "polite2", 2, NULL);

        // 一个更高优先级的 hog，会抢占并长期占用 CPU
        xTaskCreate(hog_task, "hog", 2048, NULL, 3, NULL);
    }
