#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __task {
    uint16_t counter;
    uint16_t target;
    bool enabled;
    void (*task_callback)(void);
} Task;

/*
 * See tests to figure out how the hell to invoke this function.
 *
 * I suck at C.
 */
void scheduler_init(Task **tasks, const uint8_t task_count);
void scheduler_tick(void);

#endif
