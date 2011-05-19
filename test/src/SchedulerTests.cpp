#include "CppUTest/TestHarness.h"

#include <stdio.h>

extern "C" {
    #include "scheduler.h"
}

static uint8_t noop_invocation_count;
static Task noop_task;

void noop() {
    noop_invocation_count++;
}

TEST_GROUP(SchedulerTests) {
    void setup() {
        noop_task.counter = 0;
        noop_task.target = 0;
        noop_task.enabled = true;
        noop_task.task_callback = noop;
        
        noop_invocation_count = 0;
    }
};

/*
    Task tasks[] = {
        {0, 25, true, do_something},
    };
    
    void scheduler_init(const Task **tasks);
    void scheduler_tick(void);
    void scheduler_invoke_tasks(void);
    
    for (;;) {
        scheduler_invoke_tasks();
    }

*/

TEST(SchedulerTests, QueueSingleTask) {
    const Task *tasks[] = {&noop_task};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));
    
    // hm, no testable side effects…
}

TEST(SchedulerTests, TickTimer) {
    Task task2 = {1, 42, true, noop};
    
    const Task *tasks[] = {&noop_task, &task2};

    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, noop_task.counter);
    BYTES_EQUAL(2, task2.counter);
}

TEST(SchedulerTests, TickTimerWithDisabledTask) {
    Task task2 = {1, 42, false, noop};
    
    const Task *tasks[] = {&noop_task, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, noop_task.counter);
    BYTES_EQUAL(1, task2.counter); // doesn't get incremented
}

TEST(SchedulerTests, InvokeTask) {
    const Task *tasks[] = {&noop_task};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));
    
    noop_task.counter = 3;
    noop_task.target = 4;
    
    scheduler_tick();

    scheduler_invoke_tasks();

    BYTES_EQUAL(0, noop_task.counter);
    BYTES_EQUAL(1, noop_invocation_count);
}

TEST(SchedulerTests, NoTaskInvocationWithoutTick) {
    // task must have non-zero counter to be invoked
    noop_task.counter = 10;
    noop_task.target = 10;
    
    const Task *tasks[] = {&noop_task};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));
    
    scheduler_invoke_tasks();
    
    BYTES_EQUAL(10, noop_task.counter);
    BYTES_EQUAL(0, noop_invocation_count);
}
