#include "demo_registry.h"
#include "esp_log.h"
#include "sdkconfig.h"

// forward declarations
void demo_01_run(void);
void demo_02_run(void);
void demo_03_run(void);
void demo_04_run(void);
void demo_05_run(void);
void demo_06_run(void);
void demo_07_run(void);
void demo_08_run(void);
void demo_09_run(void);

demo_id_t demo_registry_get_selected(void)
{
#if CONFIG_DEMO_01
    return DEMO_01_SCHEDULER_BASICS;
#elif CONFIG_DEMO_02
    return DEMO_02_STARVATION_TIMESLICE;
#elif CONFIG_DEMO_03
    return DEMO_03_DELAY_UNTIL;
#elif CONFIG_DEMO_04
    return DEMO_04_QUEUE_PIPELINE;
#elif CONFIG_DEMO_05
    return DEMO_05_TASK_NOTIFY;
#elif CONFIG_DEMO_06
    return DEMO_06_EVENT_GROUP;
#elif CONFIG_DEMO_07
    return DEMO_07_PRIORITY_INVERSION;
#elif CONFIG_DEMO_08
    return DEMO_08_SOFTWARE_TIMER;
#elif CONFIG_DEMO_09
    return DEMO_09_DIAGNOSTICS;
#else
    return DEMO_01_SCHEDULER_BASICS;
#endif
}

void demo_registry_run(demo_id_t id)
{
    switch (id) {
        case DEMO_01_SCHEDULER_BASICS: demo_01_run(); break;
        case DEMO_02_STARVATION_TIMESLICE: demo_02_run(); break;
        case DEMO_03_DELAY_UNTIL: demo_03_run(); break;
        case DEMO_04_QUEUE_PIPELINE: demo_04_run(); break;
        case DEMO_05_TASK_NOTIFY: demo_05_run(); break;
        case DEMO_06_EVENT_GROUP: demo_06_run(); break;
        case DEMO_07_PRIORITY_INVERSION: demo_07_run(); break;
        case DEMO_08_SOFTWARE_TIMER: demo_08_run(); break;
        case DEMO_09_DIAGNOSTICS: demo_09_run(); break;
        default: demo_01_run(); break;
    }
}
