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
            // 这段代码运行一段时间之后会出现看门口溢出
            //     task_wdt: Task watchdog got triggered
            //     task_wdt: - IDLE (CPU 0)
            //     Tasks currently running: CPU 0: C_yield
            // 这说明：CPU0 的 Idle 任务没有在规定时间内运行/喂狗，而 CPU 一直被 task_c 占着。
            // 关键原因：taskYIELD() 并不等于“让给 Idle”
            // taskYIELD() 的作用是：主动触发一次调度点，但它只是在“当前可运行任务集合”里让调度器重新选择。
            // 当系统里：
            //     A/B 都 vTaskDelay() 进入 Blocked
            //     C 一直 Ready 且几乎立刻又继续跑
            //     由于同优先级/更高优先级没有别的 ready 任务可选（或切换后马上又回到 C），调度器可能仍然选中 C
            //     Idle 得不到足够的运行机会，IDLE 没法及时执行（包含它的喂狗/清理工作）→ 任务看门狗触发
            // 解决办法：用vTaskDelay(pdMS_TO_TICKS(10));替代，阻塞10ms，避免任务C长时间占用CPU
            // 总结：
            //     实时系统中，高频循环任务必须“阻塞式设计”（delay/等待事件/等待队列），忙等 + yield 不能替代阻塞，
            //     否则可能让 Idle 长期跑不到，触发 task watchdog。
        }
    }

    void demo_01_run(void)
    {
        ESP_LOGI(TAG, "DEMO 01 start: Scheduler basics (task states)");
        xTaskCreate(task_a, "A_1s", 2048, NULL, 2, NULL);
        xTaskCreate(task_b, "B_500ms", 2048, NULL, 3, NULL);
        xTaskCreate(task_c, "C_yield", 2048, NULL, 1, NULL);
    }
