#pragma once
#include <stdbool.h>

void app_framework_init(void);

// 健康监控与诊断（可在 demo 中调用）
void app_health_start(void);
void app_health_print_once(void);

void app_stats_print_task_list_once(void);
void app_stats_print_runtime_once(void);
