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
        spy_task.target = 0;
        spy_task.enabled = true;
        spy_task.ready_to_fire = false;
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

TEST(SchedulerTests, TickTimer) {
    Task task2 = {1, 42, true, false, callback_spy};
    const Task *tasks[] = {&spy_task, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, spy_task.counter);
    BYTES_EQUAL(2, task2.counter);
}

TEST(SchedulerTests, TickTimerWithDisabledTask) {
    Task task2 = {1, 42, false, false, callback_spy};
    const Task *tasks[] = {&spy_task, &task2};
    scheduler_init((Task **)&tasks, sizeof(tasks)/sizeof(tasks[0]));

    scheduler_tick();
    
    BYTES_EQUAL(1, spy_task.counter);
    BYTES_EQUAL(1, task2.counter); // doesn't get incremented
}

TEST(SchedulerTests, TickTimerAndPrepareTaskToFire) {
    spy_task.counter = 3;
    spy_task.target = 4;
    
    scheduler_tick();
    
    CHECK_TRUE(spy_task.ready_to_fire);
    BYTES_EQUAL(0, spy_task.counter);
}

TEST(SchedulerTests, InvokeTask) {
    spy_task.counter = 3;
    spy_task.target = 4;
    
    scheduler_tick();

    CHECK_TRUE(spy_task.ready_to_fire);
    
    scheduler_invoke_tasks();

    BYTES_EQUAL(0, spy_task.counter);
    BYTES_EQUAL(1, spy_invocation_count);
}

TEST(SchedulerTests, NoTaskInvocationWithoutTick) {
    // task must have non-zero counter to be invoked
    spy_task.counter = 10;
    spy_task.target = 10;
    spy_task.ready_to_fire = false;
    
    scheduler_invoke_tasks();
    
    BYTES_EQUAL(10, spy_task.counter);
    BYTES_EQUAL(0, spy_invocation_count);
}

TEST(SchedulerTests, SkippedInvocationStillFires) {
    // ensure tasks still fire even if multiple ticks occur between invocations
    
    spy_task.counter = 9;
    spy_task.target = 10;
    
    scheduler_tick();
    CHECK_TRUE(spy_task.ready_to_fire);
    BYTES_EQUAL(0, spy_task.counter);

    scheduler_tick();
    CHECK_TRUE(spy_task.ready_to_fire);
    BYTES_EQUAL(1, spy_task.counter);

    scheduler_invoke_tasks();

    BYTES_EQUAL(1, spy_invocation_count);
    CHECK_FALSE(spy_task.ready_to_fire);
}
