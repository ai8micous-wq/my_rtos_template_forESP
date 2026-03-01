\
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"

    static const char *TAG = "demo06";

    static EventGroupHandle_t eg;

    #define WIFI_CONNECTED_BIT BIT0
    #define MQTT_CONNECTED_BIT BIT1

    static void wifi_task(void *arg)
    {
        (void)arg;
        while (1) {
            ESP_LOGI(TAG, "wifi: connecting...");
            vTaskDelay(pdMS_TO_TICKS(800));
            ESP_LOGI(TAG, "wifi: connected -> set WIFI_CONNECTED_BIT");
            xEventGroupSetBits(eg, WIFI_CONNECTED_BIT);

            // 模拟断线
            vTaskDelay(pdMS_TO_TICKS(5000));
            ESP_LOGW(TAG, "wifi: disconnected -> clear WIFI_CONNECTED_BIT");
            xEventGroupClearBits(eg, WIFI_CONNECTED_BIT);
        }
    }

    static void mqtt_task(void *arg)
    {
        (void)arg;
        while (1) {
            // 等 WiFi
            xEventGroupWaitBits(eg, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
            ESP_LOGI(TAG, "mqtt: connecting (requires WiFi)...");
            vTaskDelay(pdMS_TO_TICKS(600));
            ESP_LOGI(TAG, "mqtt: connected -> set MQTT_CONNECTED_BIT");
            xEventGroupSetBits(eg, MQTT_CONNECTED_BIT);

            // 如果 WiFi 掉线，模拟 MQTT 也掉
            xEventGroupWaitBits(eg, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
            // 继续等下一个循环
            vTaskDelay(pdMS_TO_TICKS(1000));
            if ((xEventGroupGetBits(eg) & WIFI_CONNECTED_BIT) == 0) {
                ESP_LOGW(TAG, "mqtt: WiFi lost -> clear MQTT_CONNECTED_BIT");
                xEventGroupClearBits(eg, MQTT_CONNECTED_BIT);
            }
        }
    }

    static void upload_task(void *arg)
    {
        (void)arg;
        uint32_t seq = 0;
        while (1) {
            // 只有 WiFi+MQTT 都 OK 才允许上传
            xEventGroupWaitBits(eg, WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
            ESP_LOGI(TAG, "upload: allowed (wifi+mqtt ready), publish seq=%u", (unsigned)seq++);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    void demo_06_run(void)
    {
        ESP_LOGI(TAG, "DEMO 06 start: EventGroup gate (WiFi/MQTT mock)");
        eg = xEventGroupCreate();
        configASSERT(eg);

        xTaskCreate(wifi_task, "wifi", 3072, NULL, 3, NULL);
        xTaskCreate(mqtt_task, "mqtt", 3072, NULL, 4, NULL);
        xTaskCreate(upload_task, "upload", 3072, NULL, 5, NULL);
    }
