#include <pthread.h>
#include "datatypes.h"


//======================================================================//


//======================================================================//

void* thread_sort_partition(void* input);

ENTRY* get_partition(ENTRY * table, int start, int len);

void sort_partition(ENTRY * part, int len, int id);

void print_thread_info(const char * type, int id, int start, int len);

void swap_entries(ENTRY * dest, ENTRY * src);

void split_and_sort_partition(ENTRY * table, int numprocs);

void thread_barrier (int numprocs, pthread_t * threads);
