#include "CppUTest/TestHarness.h"

#include <stdio.h>

extern "C" {
    #include "scheduler.h"
}

static uint8_t noop_invocation_count;

void noop() {
    noop_invocation_count++;
}

TEST_GROUP(SchedulerTests) {
    void setup() {
        noop_invocation_count = 0;
    }
};

/*
    Task tasks[] = {
        {0, 25, true, do_something},
    };
    
    void scheduler_init(const Task **tasks);
    void scheduler_tick(void);
    void scheduler_invoke_jobs(void);
    
    for (;;) {
        scheduler_invoke_jobs();
    }

*/

TEST(SchedulerTests, QueueSingleTask) {
    Task task1 = {0, 0, true, noop};
    
    const Task *tasks[] = {&task1};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));
    
    // hm, no testable side effects…
}

TEST(SchedulerTests, TickTimer) {
    Task task1 = {0, 42, true, noop};
    Task task2 = {1, 42, true, noop};
    
    const Task *tasks[] = {&task1, &task2};

    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, task1.counter);
    BYTES_EQUAL(2, task2.counter);
}

TEST(SchedulerTests, TickTimerWithDisabledTask) {
    Task task1 = {0, 42, true,  noop};
    Task task2 = {1, 42, false, noop};
    
    const Task *tasks[] = {&task1, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, task1.counter);
    BYTES_EQUAL(1, task2.counter); // doesn't get incremented
}

