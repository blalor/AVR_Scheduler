#include "CppUTest/TestHarness.h"

#include <stdio.h>

extern "C" {
    #include "scheduler.h"
}

static uint8_t spy_invocation_count;
static Task spy_task;
static const Task *global_tasks[] = {&spy_task};

void callback_spy() {
    spy_invocation_count++;
}

TEST_GROUP(SchedulerTests) {
    void setup() {
        spy_task.counter = 0;
        spy_task.target = 1;
        spy_task.enabled = true;
        spy_task.task_callback = callback_spy;
        
        spy_invocation_count = 0;

        scheduler_init(
            (Task **)&global_tasks,
            sizeof(global_tasks)/sizeof(global_tasks[0])
        );
    }
};

TEST(SchedulerTests, QueueSingleTask) {
    // hm, no observable side effects…
}

TEST(SchedulerTests, TickTimerAndInvokeTasks) {
    Task task2 = {1, 42, true, callback_spy};
    const Task *tasks[] = {&spy_task, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    scheduler_invoke_tasks();
    
    BYTES_EQUAL(0, spy_task.counter);
    BYTES_EQUAL(2, task2.counter);
    BYTES_EQUAL(1, spy_invocation_count);
}

TEST(SchedulerTests, TickTimerWithDisabledTask) {
    Task task2 = {1, 42, false, callback_spy};
    const Task *tasks[] = {&spy_task, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    scheduler_invoke_tasks();
    
    BYTES_EQUAL(0, spy_task.counter);
    BYTES_EQUAL(1, task2.counter); // doesn't get incremented
    BYTES_EQUAL(1, spy_invocation_count);
}

TEST(SchedulerTests, NoTaskInvocationWithoutTick) {
    // task must have non-zero counter to be invoked
    spy_task.counter = 10;
    spy_task.target = 10;
    
    scheduler_invoke_tasks();
    
    BYTES_EQUAL(10, spy_task.counter);
    BYTES_EQUAL(0, spy_invocation_count);
}

TEST(SchedulerTests, SkippedInvocationStillFires) {
    // ensure tasks still fire even if multiple ticks occur between invocations
    
    // expect counter increment commensurate with number of ticks
    spy_task.counter = 7;
    spy_task.target = 10;
    
    scheduler_tick();
    scheduler_tick();
    
    scheduler_invoke_tasks();
    BYTES_EQUAL(9, spy_task.counter);
    BYTES_EQUAL(0, spy_invocation_count);
    
    scheduler_tick();
    
    scheduler_invoke_tasks();
    BYTES_EQUAL(0, spy_task.counter);
    BYTES_EQUAL(1, spy_invocation_count);
}

TEST(SchedulerTests, MissedTriggerStillFires) {
    // ensure tasks still fire even if multiple ticks occur between invocations
    
    spy_task.counter = 9;
    spy_task.target = 10;
    
    scheduler_tick();
    scheduler_tick();
    scheduler_tick();
    
    scheduler_invoke_tasks();
    BYTES_EQUAL(0, spy_task.counter);
    BYTES_EQUAL(1, spy_invocation_count);
}
