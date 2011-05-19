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
    tick_occurred = false;
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
        uint16_t counter, target;
        
        for (int i = 0; i < task_count; i++) {
            counter = tasks[i]->counter;
            target = tasks[i]->target;
            
            if (
                tasks[i]->enabled &&
                (counter > 0) &&
                (counter >= target)
            ) {
                // counter could be reset before or after task invocation. If 
                // done before, task is started at (approximately) regular 
                // intervals.  If done after, task is started after its scheduled
                // period after execution.
                tasks[i]->counter = 0;
                tasks[i]->task_callback();
            }
        }
    }
}
