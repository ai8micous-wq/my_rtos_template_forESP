\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include <string.h>

    static const char *TAG = "demo04";

    typedef struct {
        uint32_t seq;
        int32_t  value;
        TickType_t tick;
    } sample_t;

    static QueueHandle_t q;

    static void sample_task(void *arg)
    {
        (void)arg;
        TickType_t last = xTaskGetTickCount();
        uint32_t seq = 0;

        while (1) {
            sample_t s = {
                .seq = seq++,
                .value = (int32_t)(seq * 3),
                .tick = xTaskGetTickCount(),
            };
            // 模拟采样耗时
            vTaskDelay(pdMS_TO_TICKS(10));

            if (xQueueSend(q, &s, 0) != pdTRUE) {
                ESP_LOGW(TAG, "queue full: drop seq=%u", (unsigned)s.seq);
            } else {
                ESP_LOGI(TAG, "sample -> queue: seq=%u value=%ld", (unsigned)s.seq, (long)s.value);
            }

            vTaskDelayUntil(&last, pdMS_TO_TICKS(200)); // 200ms采样
        }
    }

    static void upload_task(void *arg)
    {
        (void)arg;
        while (1) {
            sample_t s;
            if (xQueueReceive(q, &s, portMAX_DELAY) == pdTRUE) {
                // 模拟上传耗时（比如 MQTT publish）
                vTaskDelay(pdMS_TO_TICKS(60));
                ESP_LOGI(TAG, "upload <- queue: seq=%u age=%lu ticks", (unsigned)s.seq, (unsigned long)(xTaskGetTickCount() - s.tick));
            }
        }
    }

    void demo_04_run(void)
    {
        ESP_LOGI(TAG, "DEMO 04 start: Queue pipeline (sample->upload)");
        q = xQueueCreate(8, sizeof(sample_t));
        configASSERT(q);

        // 采样任务优先级高于上传，避免采样抖动
        xTaskCreate(sample_task, "sample", 3072, NULL, 6, NULL);
        xTaskCreate(upload_task, "upload", 3072, NULL, 4, NULL);
    }
