/*
mlfqs2-roundrobin

3 jobs enter queue and don't use up whole q so that they run in round robin fashion for 5 iterations
*/

#include <stdio.h>
#include "tests/threads/tests.h" 
#include "threads/init.h"
#include "devices/timer.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct thread_data 
  {
    int id;                     /* Sleeper ID. */
    int priority;
    int iterations;             /* Iterations so far. */
    struct lock *lock;          /* Lock on output. */
    int **op;                   /* Output buffer position. */
  };

// 3 threads that all run for 5 iterations and are alloted 1 quantum each
#define THREAD_CNT 3
#define ITER_CNT 5
#define QUANTUM_TICKS 1        

static thread_func simple_thread_func;

static void test_mlfqs2_roundrobin(void){
	struct thread_data data[THREAD_CNT];
    struct lock lock;

    msg ("%d threads will iterate %d times running round robin.", THREAD_CNT, ITER_CNT);

    lock_init (&lock);
    lock_acquire(&lock); // main holds lock
    printf("Main at priority %d\n",PRI_MIN);
    thread_set_priority (PRI_MIN); // main at min priority

	for (i = 0; i < THREAD_CNT; i++) {
		char name[16];
		struct thread_data *d = data + i;
		snprintf (name, sizeof name, "%d", i);
		//d->tick_count = 0;
		d->priority = PRI_DEFAULT;
		d->id = i;
		d->iterations = 0;
		d->lock = &lock; // all share one lock
		thread_create (name, PRI_DEFAULT, test_short, d);
	}

	msg("Main thread yielding...");
	lock_release(&lock); // all threads can run
	thread_yield();

	timer_sleep(20);
    
    msg("Main thread resumed");
    
    //order of execution
    lock_acquire(&lock);
    msg("Execution order:");
    for (i = 0; i < order_index && i < THREAD_CNT * ITER_CNT; i++) {
        msg("  Step %d: Thread %d", i + 1, execution_order[i]);
    }
    
    // show counts
    msg("\nIteration counts:");
    bool all_completed = true;
    for (i = 0; i < THREAD_CNT; i++) {
        msg("  Thread %d: %d iterations", data[i].id, data[i].iterations);
        if (data[i].iterations != ITER_CNT) {
            all_completed = false;
        }
    }
    
    // verify round robin
    bool is_fair = true;
    int thread_counts[THREAD_CNT];
    for (i = 0; i < THREAD_CNT; i++) {
        thread_counts[i] = 0;
    }
    
    for (i = 0; i < order_index && i < THREAD_CNT * ITER_CNT; i++) {
        int tid = execution_order[i];
        if (tid >= 0 && tid < THREAD_CNT) {
            thread_counts[tid]++;
            
            //no thread should be more than 1 ahead
            int min_count = thread_counts[0];
            int max_count = thread_counts[0];
            for (int j = 1; j < THREAD_CNT; j++) {
                if (thread_counts[j] < min_count) min_count = thread_counts[j];
                if (thread_counts[j] > max_count) max_count = thread_counts[j];
            }
            
            if (max_count - min_count > 1) {
                is_fair = false;
                break;
            }
        }
    }
    
    // results
    if (all_completed && is_fair) {
        msg("PASS: All threads completed %d iterations in round-robin fashion", ITERATIONS);
    } else if (!all_completed) {
        msg("FAIL: Not all threads completed %d iterations", ITER_CNT);
    } else {
        msg("FAIL: Scheduling was not fair round-robin");
    }
    
    lock_release(&lock);
    msg("Test complete.");
}

static void 
simple_thread_func (void *data) 
{
  struct simple_thread_data *data = data;
  int i;

  for (i = 0; i < ITER_CNT; i++) 
    {
      lock_acquire (data->lock);
      msg("Current thread running %d", data->id);
      msg("Thread running with priority %d", data->initial_priority);
      *(*data->op)++ = data->id;

      lock_release (data->lock);
      thread_yield ();
    }
}

