\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"

    static const char *TAG = "demo07";
    static SemaphoreHandle_t lock;

    static TaskHandle_t low_handle = NULL;
    static TaskHandle_t high_handle = NULL;
    static TaskHandle_t med_handle = NULL;

    /*
      目的：制造经典优先级反转：
        - Low  任务先拿到 mutex，做“长工作”
        - High 任务很快来抢 mutex，被阻塞
        - Medium 任务不需要 mutex，但会持续运行，导致 Low 得不到 CPU 释放锁
      结果：High 被 Medium “间接阻塞” —— 优先级反转。

      如果 mutex 支持优先级继承（FreeRTOS 的 mutex 支持），Low 会临时提升到 High 的优先级，
      从而抢回 CPU、尽快释放锁，High 受影响时间大幅缩短。
    */

    // 观测辅助：打印任务优先级（继承发生时 LOW 会临时变高）
    static void log_prio(const char *who, TaskHandle_t h)
    {
        UBaseType_t prio = uxTaskPriorityGet(h);
        ESP_LOGI(TAG, "%s: current prio=%u", who, (unsigned)prio);
    }

    static void low_task(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "LOW: take mutex, do long work (simulate 2000ms)");
            xSemaphoreTake(lock, portMAX_DELAY);

            // 观测日志 #1：拿到锁时的优先级（通常为 1，若已继承则会更高）
            log_prio("LOW(after take)", low_handle);

            // 模拟“长临界区” 2000ms
            TickType_t start = xTaskGetTickCount();
            while (xTaskGetTickCount() - start < pdMS_TO_TICKS(2000)) {
                // 每 200ms 打一次优先级，观察是否被继承抬升
                TickType_t elapsed = xTaskGetTickCount() - start;
                if ((elapsed % pdMS_TO_TICKS(200)) == 0) {
                    log_prio("LOW(holding lock)", low_handle);
                }                
                // 故意不 delay：让它吃 CPU，这样更容易观察“继承后被提升”效果
                for (volatile int i = 0; i < 10000; i++) { }
                taskYIELD();
            }

            // 观测日志：释放前优先级（如果发生继承，释放前应仍是提升态；释放后会恢复）
            log_prio("LOW(before give)", low_handle);
            
            ESP_LOGI(TAG, "LOW: give mutex");
            xSemaphoreGive(lock);

            // give 之后再打印一次，看是否恢复原优先级
            log_prio("LOW(after give)", low_handle);

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

            // 观测日志 #2：等待 ticks + 换算 ms（你的 tick=10ms）
            ESP_LOGW(TAG, "HIGH: got mutex after waiting %lu ticks (~%lu ms), do short work 50ms",
                     (unsigned long)waited,
                     (unsigned long)(waited * (1000 / configTICK_RATE_HZ)));
            
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

        xTaskCreate(low_task, "low", 4096, NULL, 1, &low_handle);
        xTaskCreate(medium_task, "med", 4096, NULL, 2, &med_handle);
        xTaskCreate(high_task, "high", 4096, NULL, 3, &high_handle);
    }
