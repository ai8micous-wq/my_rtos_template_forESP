\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/timers.h"

    static const char *TAG = "demo08";
    static TimerHandle_t tmr;

    static void timer_cb(TimerHandle_t xTimer)
    {
        (void)xTimer;
        // 注意：这里运行在 Timer Service Task 上，不要做耗时/阻塞操作
        static uint32_t n = 0;
        ESP_LOGI(TAG, "timer_cb: tick #%u", (unsigned)n++);
    }

    static void worker_task(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "worker: do work 700ms (independent of timer)");
            vTaskDelay(pdMS_TO_TICKS(700));
        }
    }

    void demo_08_run(void)
    {
        ESP_LOGI(TAG, "DEMO 08 start: Software Timer");
        tmr = xTimerCreate("tmr", pdMS_TO_TICKS(200), pdTRUE, NULL, timer_cb);
        configASSERT(tmr);
        xTimerStart(tmr, 0);

        xTaskCreate(worker_task, "worker", 3072, NULL, 2, NULL);
    }
