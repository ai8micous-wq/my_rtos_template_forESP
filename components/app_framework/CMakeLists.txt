idf_component_register(
    SRCS "app_framework.c" "app_health.c" "app_stats.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_timer
)
