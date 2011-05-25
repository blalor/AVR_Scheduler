#ifndef SCHEDULER_H
#define SCHEDULER_H

/*
 Simple task scheduler.
 
 Configure a timer to execute scheduler_tick() periodically.  Call
 scheduler_invoke_tasks() from main loop.
 
 scheduler_tick() could be invoked via any timer interrupt (overflow or compare)
 
 Ticks occur at (F_CPU/timer_prescaler)/timer_overflow_or_compare.
 
 Example:
     F_CPU = 8000000UL (8 MHz)
     prescale = 64
     timer incremented every 8 µS
     timer overflow every 256 ticks
     
     scheduler_tick() invoked on overflow:
         (8 µS * 256) = 2048 µS per counter increment
         (2048 µS * 256) = 524288 µS max task period uint8_t
         (2048 µS * 65535) = 134215680 µS / 134.2 S max task period uint16_t
 
 The table below shows the maximum counter values that can be achieved for a 
 given prescaler and counter/target size.
 
 F_CPU: 8,000,000 (8 MHz)
 +----------+------------+-----------+--------------+---------------+----------------+----------------+
 |          |            |  µS/cycle |              |              S  E  C  O  N  D  S                |
 | prescale |        Hz  |   (1/Hz)  | µS/TCNTx OVR | max - uint8_t | max - uint16_t | max - uint32_t |
 +----------+------------+-----------+--------------+---------------+----------------+----------------+
 |        1 | 8,000,000  |     0.125 |           32 |         0.008 |          2.097 |        137,439 |
 |        2 | 4,000,000  |     0.250 |           64 |         0.016 |          4.194 |        274,878 |
 |        4 | 2,000,000  |     0.500 |          128 |         0.033 |          8.388 |        549,756 |
 |        8 | 1,000,000  |     1.000 |          256 |         0.065 |         16.777 |      1,099,512 |
 |       16 |   500,000  |     2.000 |          512 |         0.131 |         33.554 |      2,199,023 |
 |       32 |   250,000  |     4.000 |        1,024 |         0.261 |         67.108 |      4,398,047 |
 |       64 |   125,000  |     8.000 |        2,048 |         0.522 |        134.216 |      8,796,093 |
 |      128 |    62,500  |    16.000 |        4,096 |         1.044 |        268.431 |     17,592,186 |
 |      256 |    31,250  |    32.000 |        8,192 |         2.089 |        536.863 |     35,184,372 |
 |      512 |    15,625  |    64.000 |       16,384 |         4.178 |      1,073.725 |     70,368,744 |
 |    1,024 |    7,812.5 |   128.000 |       32,768 |         8.356 |      2,147.451 |    140,737,488 |
 |    2,048 |    3,906.2 |   256.000 |       65,536 |        16.712 |      4,294.902 |    281,474,977 |
 |    4,096 |    1,953.1 |   512.000 |      131,072 |        33.423 |      8,589.804 |    562,949,953 |
 |    8,192 |      976.6 | 1,024.000 |      262,144 |        66.847 |     17,179.607 |  1,125,899,907 |
 |   16,384 |      488.3 | 2,048.000 |      524,288 |       133.693 |     34,359.214 |  2,251,799,813 |
 +----------+------------+-----------+--------------+---------------+----------------+----------------+
*/

#include <stdint.h>
#include <stdbool.h>

// compile-time type selection allows for easier modification per table above
#define COUNTER_TIMER_TYPE uint16_t

/* for future use; stolen from Arduino's wiring.h
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define microsecondsToClockCycles(a) ( ((a) * (F_CPU / 1000L)) / 1000L )
*/

#define usecToTaskTarget(prescale, timer_tick, us) (us / ((prescale * timer_tick * 1000000UL)/F_CPU))
#define secToTaskTarget(prescale, timer_tick, s) usecToTaskTarget(prescale, timer_tick, (s * 1000000UL))

typedef struct __task {
    COUNTER_TIMER_TYPE counter;
    COUNTER_TIMER_TYPE target;
    bool enabled;
    void (*task_callback)(void);
} Task;

/*
 * See tests to figure out how the hell to invoke this function.  I do not 
 * know why it works, I'm just glad it works.
 *
 * I suck at C.
 */
void scheduler_init(Task **tasks, const uint8_t task_count);

/*
 * Invoked periodically by the timer.
 */
void scheduler_tick(void);

/*
 * Called in the main execution loop. Invokes tasks when they're ready.
 */
void scheduler_invoke_tasks(void);

#endif
