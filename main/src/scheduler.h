#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __task {
    volatile uint16_t counter;
    uint16_t target;
    volatile bool enabled;
    void (*task_callback)(void);
} Task;

/*
 * See tests to figure out how the hell to invoke this function.  I do not 
 * know why it works, I'm just glad it works.
 *
 * I suck at C.
 */
void scheduler_init(Task **tasks, const uint8_t task_count);
void scheduler_tick(void);
void scheduler_invoke_tasks(void);

#endif
