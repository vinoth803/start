// C code to synchronize threads
#include "common.h"
#include <pthread.h>

void thread_init();
int thread_start();
void* thread_func(void* arg);

