'''
mlfqs2-leavebforeq

Job enters queue, leaves before 1 quantum
'''

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
#define ITER_CNT 1

static thread_func simple_thread_func;

static void test_leavebeforeq(void){
	struct simple_thread_data data;
	struct lock lock;
	int output[THREAD_CNT * ITER_CNT];
	
	lock_init (&lock);
	lock_acquire(&lock); 

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
	timer_seep(10);

	msg("Main thread resumed");

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

      *(*data->op)++ = data->id;
      lock_release (data->lock);
      thread_yield ();
    }
}