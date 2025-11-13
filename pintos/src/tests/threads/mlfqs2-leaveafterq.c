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
#define ITER_CNT 1