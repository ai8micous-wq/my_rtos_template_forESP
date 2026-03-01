\
    #include <stdio.h>
    #include "esp_log.h"
    #include "nvs_flash.h"
    
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    
    #include "app_framework.h"
    #include "demo_registry.h"

    static const char *TAG = "app_main";

    void app_main(void)
    {
        // NVS init（很多 ESP-IDF 组件会依赖）
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ESP_ERROR_CHECK(nvs_flash_init());
        }

        app_framework_init();

        demo_id_t id = demo_registry_get_selected();
        ESP_LOGI(TAG, "Selected demo id=%d", (int)id);

        demo_registry_run(id);

        // 兜底：如果 demo 返回，保持主线程不退出
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
