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
            // 直接运行的现象：两个polite_task任务仅在开机的时候运行了一次，随后一直卡死，看门狗一直溢出
            //
            // 注意：此处故意不 delay；如果同优先级、并开启 time slicing，才会轮转
            // hog_task没有释放运行权，一直处于running状态，同时它的优先级最高，没有其他高优先级任务可以打断它
            // 所以hog_task一直占用着CPU，知道任务调度看门狗饿死，触发异常。
            // 备注：一般触发这个异常，不会重启整个系统，只会打印异常信息，打印完后hog_task还是会一直占用CPU,
            // 所以每隔一段时间打印一次看门狗饿死异常的信息。
            // 正常的一个任务，应该要进入阻塞状态，即释放运行权，进入Blocked状态，让任务调度器调度其它低优先级的任务运行
            // 应该加入这一行：vTaskDelay(pdMS_TO_TICKS(10)); 
            //
            // 实验2，如果把hog_task的优先级设置成2，即与两个polite_task任务相同优先级，会发生什么？
        }
    }

    void demo_02_run(void)
    {
        ESP_LOGI(TAG, "DEMO 02 start: Starvation & timeslice");
        // 两个同优先级任务，看看 time-slicing 影响（取决于配置）
        xTaskCreate(polite_task, "polite1", 2048, "polite1", 2, NULL);
        xTaskCreate(polite_task, "polite2", 2048, "polite2", 2, NULL);

        // 一个更高优先级的 hog，会抢占并长期占用 CPU
        xTaskCreate(hog_task, "hog", 2048, NULL, 2, NULL);
        // 实验2，hog_task改为2
    }
