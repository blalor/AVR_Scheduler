#include "scheduler.h"

#include <stdint.h>
#include <stdbool.h>

static Task **tasks;
static uint8_t task_count;

// counter incremented when scheduler_tick runs; prevents overhead when 
// scheduler_invoke_tasks runs.
static volatile uint8_t tick_count;

void scheduler_init(Task **task_ptr_arr, const uint8_t tc) {
    tasks = task_ptr_arr;
    task_count = tc;
    
    tick_count = 0;
}

void scheduler_tick() {
    tick_count++;
}

void scheduler_invoke_tasks() {
    // note: it is possible for tick_count to get incremented between being
    // read and being set to zero…
    uint8_t tick_count_cpy = tick_count;
    tick_count = 0;
    
    COUNTER_TIMER_TYPE counter;

    if (tick_count_cpy > 0) {
        for (int i = 0; i < task_count; i++) {
            if (tasks[i]->enabled) {
                // read and increment counter
                counter = tasks[i]->counter + tick_count_cpy;

                // need to check for counter > 0 since counter could possibly 
                // be modified outside of scheduler_invoke_tasks() and could
                // roll over to 0
                if ((counter > 0) && (counter >= tasks[i]->target)) {
                    // counter could be reset before or after task invocation. 
                    // If done before, task is started at (approximately) 
                    // regular intervals.  If done after, task is started after
                    // its scheduled period after execution.
                    counter = 0;

                    tasks[i]->task_callback();
                }

                tasks[i]->counter = counter;
            }
        }
    }
}
