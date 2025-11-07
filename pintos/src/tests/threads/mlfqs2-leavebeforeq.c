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
#define ITER_CNT /* ?? */

static thread_func simple_thread_func;

static void test_leavebeforeq(void){

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