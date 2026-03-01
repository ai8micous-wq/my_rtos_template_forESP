idf_component_register(
    SRCS
        "demo_registry.c"
        "demo_01_scheduler_basics.c"
        "demo_02_starvation_timeslice.c"
        "demo_03_delay_until.c"
        "demo_04_queue_pipeline.c"
        "demo_05_task_notify.c"
        "demo_06_event_group.c"
        "demo_07_priority_inversion.c"
        "demo_08_software_timer.c"
        "demo_09_diagnostics.c"
    INCLUDE_DIRS "include"
    REQUIRES app_framework
)
