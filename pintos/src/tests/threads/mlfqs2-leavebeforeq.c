/*
mlfqs2-leavebforeq

Job enters queue, leaves before 1 quantum
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
    //int **op;                   /* Output buffer position. */
	int initial_priority;
    int final_priority;
  };

#define THREAD_CNT 1
#define ITER_CNT 1

static thread_func simple_thread_func;

void test_mlfqs2_leavebeforeq(void){
	struct simple_thread_data data;
	struct lock lock;
	
	lock_init (&lock);
	data.lock = &lock;
	data.id = 1;
	data.iterations = 0;
	data.initial_priority = PRI_DEFAULT;
    data.final_priority = -1;

	//set main priority to minimum to let thread run
	printf("Main at priority %d\n",PRI_MIN);
	thread_set_priority (PRI_MIN);

	//create a thread with default priority 
	msg("Creating one short-lived thread.");
	thread_create("short", PRI_DEFAULT, simple_thread_func, &data);

	//yield to this thread
	msg("Main thread yielding...");
	thread_yield();

	//wait for thread to finish
	timer_sleep(10);

	msg("Main thread resumed");

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

	msg("Test done. Thread should have finished before its quantum expired.");

}

static void 
simple_thread_func (void *data_) 
{
  struct simple_thread_data *data = data_;
  int i;

  for (i = 0; i < ITER_CNT; i++) 
    {
      lock_acquire (data->lock);
      data->iterations++;
	  data->initial_priority = thread_get_priority();	
      msg("Thread iteration %d complete", i + 1);
	  data->final_priority = thread_get_priority();
      lock_release (data->lock);
      thread_yield ();
    }
}
