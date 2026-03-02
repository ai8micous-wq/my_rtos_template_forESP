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
            // hog_task没有释放运行权，一直处于Running状态，同时它的优先级最高，没有其他高优先级任务可以打断它
            // 所以hog_task一直占用着CPU，知道任务调度看门狗饿死，触发异常。
            // 备注：一般触发这个异常，不会重启整个系统，只会打印异常信息，打印完后hog_task还是会一直占用CPU,
            //      所以每隔一段时间打印一次看门狗饿死异常的信息。
            // 正常的一个任务，应该要进入阻塞状态，即释放运行权，进入Blocked状态，让任务调度器调度其它低优先级的任务运行
            // 应该加入这一行：vTaskDelay(pdMS_TO_TICKS(10)); 
            // 结论：高优先级任务只要不阻塞，就能永久占用 CPU，导致低优先级任务永远得不到运行机会。
            //
            // 实验2，如果把hog_task的优先级设置成2，即与两个polite_task任务相同优先级，会发生什么？
            // 现象：polite1和polite2轮流打印日志，一段时间后看门狗饿死，触发异常；然后polite1和polite2轮流打印日志，看门狗饿死
            //       一直循环这个现象
            // 原因：现在hog，polite1和polite2都是相同优先级的任务，刚开机时，都处于Ready状态，系统按顺序执行一个
            //       等下一次，调度器Tick ISR中断到来（一般是1ms一次），调度器检查队列发现还有Ready的任务，立刻切换下一个。
            //       因此，这三个任务都可以正常运行，这个动作就是时间片轮转（在同优先级下，可以解决一部分hog占用CPU的问题）
            //       区别在于：polite1和polite2每隔200ms才能Ready()一次，其余时间都在Blocked阻塞状态，
            //                而hog一直在Runing和Ready()状态，疯狂占用CPU。
            //       由于IDLE (CPU0)的优先级低，所以它根本无法参与上面的时间片轮转，因为它的优先级低，无法抢占高优先级的任务。
            //       所以任务调度看门狗就饿死了，就报异常了。
            // 结论：时间片轮转的原理如上，每次Tick ISR()都会轮转下一个Ready的任务，但还是会优先安排高优先级的任务。
            // 
            // 延伸：如果polite1和polite2任务执行时间比较长，假设执行时间是5ms，又都处于Ready状态，
            //       tick ISR是1ms一次。因此很可能polite1刚执行了1ms（还没执行完），tick ISR触发，
            //       此时polite1就要被迫停止了，转而执行polite2任务，此时polite1的状态是Ready吗
            // 回答：是的，polite1的状态是Ready状态。因为时间片轮转是调度单位是“tick 级别的时间片”，不是“函数执行完整性”
            //       并不会等polite1执行完，才切换下一个任务。只要队列里面又Ready的任务立马切换，而当前任务把状态标志为Ready
            //       放在队列后面。
            //       就像高优先级的的任务，只要处于Ready状态，会tick到来时，立刻抢占低优先级的任务，即使它还没有执行完。
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
        // 实验2，hog_task改为2（原来为3）
    }
