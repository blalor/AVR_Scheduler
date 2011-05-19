#include "scheduler.h"

#include <stdint.h>
#include <stdbool.h>

static Task **tasks;
static uint8_t task_count;

// flag set when scheduler_tick runs; prevents overhead when 
// scheduler_invoke_tasks runs.
static volatile bool tick_occurred;

void scheduler_init(Task **task_ptr_arr, const uint8_t tc) {
    tasks = task_ptr_arr;
    task_count = tc;
    tick_occurred = false;
    
    for (int i = 0; i < task_count; i++) {
        tasks[i]->ready_to_fire = false;
    }
}

/*
    It was a toss-up between resetting the counter values to zero in the
    _tick() (invoked via ISR) or in _invoke_tasks(). If done in
    _invoke_tasks() and COUNTER_TIMER_TYPE is bigger than 8 bits, I'd have to
    use something like ATOMIC_BLOCK[1] for the reads of the counter variables.
    I chose doing all of the work in _tick() to minimize the number of times 
    interrupts get disabled.  Hopefully this function is still lean enough 
    that no interrupts get missed.
    
    [1] http://www.nongnu.org/avr-libc/user-manual/group__util__atomic.html
*/
void scheduler_tick() {
    COUNTER_TIMER_TYPE counter;
    
    for (int i = 0; i < task_count; i++) {
        if (tasks[i]->enabled) {
            // read and increment counter
            counter = tasks[i]->counter + 1;
            
            // need to check for counter > 0 since counter could possibly be
            // modified outside of scheduler_tick().  It may be desirable to
            // further check if ready_to_fire is still true before 
            // incrementing the counter, which would indicate that the task 
            // has not yet executed or is still executing
            if ((counter > 0) && (counter == tasks[i]->target)) {
                // counter could be reset before or after task invocation. If 
                // done before, task is started at (approximately) regular 
                // intervals.  If done after, task is started after its
                // scheduled period after execution.
                counter = 0;
                
                tasks[i]->ready_to_fire = true;
            }
            
            tasks[i]->counter = counter;
        }
    }
    
    tick_occurred = true;
}

void scheduler_invoke_tasks() {
    // the tick_occurred guard should keep us from relentlessly spinning 
    // through the task list between invocations of scheduler_tick()
    if (tick_occurred) {
        tick_occurred = false;

        for (int i = 0; i < task_count; i++) {
            if (tasks[i]->enabled && tasks[i]->ready_to_fire) {
                tasks[i]->task_callback();
                tasks[i]->ready_to_fire = false;
            }
        }
    }
}
