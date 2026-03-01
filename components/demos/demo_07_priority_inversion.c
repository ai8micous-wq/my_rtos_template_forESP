\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"

    static const char *TAG = "demo07";
    static SemaphoreHandle_t lock;

    /*
      目的：制造经典优先级反转：
        - Low  任务先拿到 mutex，做“长工作”
        - High 任务很快来抢 mutex，被阻塞
        - Medium 任务不需要 mutex，但会持续运行，导致 Low 得不到 CPU 释放锁
      结果：High 被 Medium “间接阻塞” —— 优先级反转。

      如果 mutex 支持优先级继承（FreeRTOS 的 mutex 支持），Low 会临时提升到 High 的优先级，
      从而抢回 CPU、尽快释放锁，High 受影响时间大幅缩短。
    */

    static void low_task(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "LOW: take mutex, do long work (simulate 2000ms)");
            xSemaphoreTake(lock, portMAX_DELAY);

            // 模拟“长临界区”
            TickType_t start = xTaskGetTickCount();
            while (xTaskGetTickCount() - start < pdMS_TO_TICKS(2000)) {
                // 故意不 delay：让它吃 CPU，这样更容易观察“继承后被提升”效果
                for (volatile int i = 0; i < 10000; i++) { }
                taskYIELD();
            }

            ESP_LOGI(TAG, "LOW: give mutex");
            xSemaphoreGive(lock);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    static void high_task(void *arg)
    {
        (void)arg;
        vTaskDelay(pdMS_TO_TICKS(300)); // 确保 LOW 先拿到锁
        while (1) {
            ESP_LOGW(TAG, "HIGH: try take mutex (will block if LOW holds it)");
            TickType_t t0 = xTaskGetTickCount();
            xSemaphoreTake(lock, portMAX_DELAY);
            TickType_t waited = xTaskGetTickCount() - t0;

            ESP_LOGW(TAG, "HIGH: got mutex after waiting %lu ticks, do short work 50ms", (unsigned long)waited);
            vTaskDelay(pdMS_TO_TICKS(50));
            xSemaphoreGive(lock);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    static void medium_task(void *arg)
    {
        (void)arg;
        vTaskDelay(pdMS_TO_TICKS(500)); // 让 HIGH 先进入阻塞
        while (1) {
            // 中等优先级“忙碌任务”，不需要 mutex
            ESP_LOGI(TAG, "MED: busy 300ms (no mutex)");
            TickType_t start = xTaskGetTickCount();
            while (xTaskGetTickCount() - start < pdMS_TO_TICKS(300)) {
                for (volatile int i = 0; i < 20000; i++) { }
                // 不 delay
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }

    void demo_07_run(void)
    {
        ESP_LOGI(TAG, "DEMO 07 start: Priority inversion & inheritance (mutex)");
        lock = xSemaphoreCreateMutex(); // 关键：mutex 才有优先级继承；binary semaphore 没有
        configASSERT(lock);

        xTaskCreate(low_task, "low", 4096, NULL, 1, NULL);
        xTaskCreate(medium_task, "med", 4096, NULL, 2, NULL);
        xTaskCreate(high_task, "high", 4096, NULL, 3, NULL);
    }
