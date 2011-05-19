#include "scheduler.h"

#include <stdint.h>
#include <stdbool.h>

static Task **tasks;
static uint8_t task_count;

// flag set when scheduler_tick runs; prevents overhead when 
// scheduler_invoke_tasks runs.
static bool tick_occurred;

void scheduler_init(Task **task_ptr_arr, const uint8_t tc) {
    tasks = task_ptr_arr;
    task_count = tc;
}

void scheduler_tick() {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i]->enabled) {
            tasks[i]->counter++;
        }
    }
    
    tick_occurred = true;
}

void scheduler_invoke_tasks() {
    if (tick_occurred) {
        for (int i = 0; i < task_count; i++) {
            if (
                tasks[i]->enabled &&
                (tasks[i]->counter > 0) &&
                (tasks[i]->counter == tasks[i]->target)
            ) {
                tasks[i]->counter = 0;

                tasks[i]->task_callback();
            }
        }
    }
}
