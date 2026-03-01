    #include "app_framework.h"
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static const char *TAG = "app_stats";

    void app_stats_print_task_list_once(void)
    {
    #if (configUSE_TRACE_FACILITY == 1)
        static char buf[2048];
        vTaskList(buf);
        ESP_LOGI(TAG, "Task List:
Name          State  Prio Stack#  Num
%s", buf);
    #else
        ESP_LOGW(TAG, "configUSE_TRACE_FACILITY disabled");
    #endif
    }

    void app_stats_print_runtime_once(void)
    {
    #if (configGENERATE_RUN_TIME_STATS == 1)
        static char buf[2048];
        vTaskGetRunTimeStats(buf);
        ESP_LOGI(TAG, "Runtime Stats:
Task            Time     %%
%s", buf);
    #else
        ESP_LOGW(TAG, "configGENERATE_RUN_TIME_STATS disabled");
    #endif
    }
