/*
mlfqs2-leaveafterq

Job enters queue, leaves after 1 quantum and should have a lower priority as a result
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "devices/timer.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct simple_thread_data 
  {
    int id;                     /* Sleeper ID. */
    int iterations;             /* Iterations so far. */
    struct lock *lock;          /* Lock on output. */
    int **op;                   /* Output buffer position. */
  };

#define THREAD_CNT 1
#define QUANTUM_TICKS 1 

static thread_func simple_thread_func;

static void test_leaveafterq(void){
	struct simple_thread_data data;
    struct lock lock;

    lock_init (&lock);
    data.lock = &lock;
    data.id = 1;
    data.initial_priority = PRI_DEFAULT;
    data.final_priority = -1;

    msg("Main thread setting priority to %d", PRI_MIN);
    thread_set_priority (PRI_MIN);

    msg("Creating thread that will run for more than 1 quantum.");
    thread_create("long", PRI_DEFAULT, simple_thread_func, &data);

    msg("Main thread yielding...");
    thread_yield();

    /* wait for more than 1 quantum for thread to finish */
    timer_sleep(QUANTUM_TICKS + 2);

    msg("Main thread resumed");

    /*Check if thread priority was reduced*/
    lock_acquire(&lock);
    if (data.final_priority != -1) {
        msg("Thread initial priority: %d", data.initial_priority);
        msg("Thread final priority: %d", data.final_priority);
        
        if (data.final_priority < data.initial_priority) {
            msg("PASS: Thread priority was lowered after quantum expired.");
        } else {
            msg("FAIL: Thread priority was not lowered. Expected < %d, got %d",
                data.initial_priority, data.final_priority);
        }
    } else {
        msg("FAIL: Thread did not record final priority.");
    }
    lock_release(&lock);

    msg("Test complete.");
}

static void 
simple_thread_func (void *data_){

	struct simple_thread_data *data = data_;
    int64_t start_tick;
    
    lock_acquire (data->lock);
    data->initial_priority = thread_get_priority();
    msg("Thread started with priority %d", data->initial_priority);
    lock_release (data->lock);

    start_tick = timer_ticks();
    
    /* Busy loop for more than 1 quantum*/
    while (timer_elapsed(start_tick) < QUANTUM_TICKS + 1) {
    }

    lock_acquire (data->lock);
    data->final_priority = thread_get_priority();
    msg("Thread ending with priority %d after %lld ticks",data->final_priority, timer_elapsed(start_tick));
    lock_release (data->lock);
}