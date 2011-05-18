#include "scheduler.h"

static Task **tasks;
static uint8_t task_count;

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
}
