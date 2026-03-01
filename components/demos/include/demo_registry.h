#pragma once

typedef enum {
    DEMO_01_SCHEDULER_BASICS = 1,
    DEMO_02_STARVATION_TIMESLICE,
    DEMO_03_DELAY_UNTIL,
    DEMO_04_QUEUE_PIPELINE,
    DEMO_05_TASK_NOTIFY,
    DEMO_06_EVENT_GROUP,
    DEMO_07_PRIORITY_INVERSION,
    DEMO_08_SOFTWARE_TIMER,
    DEMO_09_DIAGNOSTICS,
} demo_id_t;

demo_id_t demo_registry_get_selected(void);
void demo_registry_run(demo_id_t id);
