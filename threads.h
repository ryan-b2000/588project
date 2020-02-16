#include <pthread.h>
#include "datatypes.h"


//======================================================================//
typedef struct {
	int id;									// the thread ID
	int start;							// start of partition
	int len;								// length of partition
	ENTRY * table;		// pointer to master table
} THREAD_ARGS;



//======================================================================//

void* thread_sort_partition(void* input);


ENTRY* get_partition(ENTRY * table, int start, int len);

void sort_partition(ENTRY * part, int len, int id);

void print_thread_info(THREAD_ARGS args);

void swap_entries(ENTRY * dest, ENTRY * src);

void split_and_sort_partition(ENTRY * table, int numprocs);

void thread_barrier (int numprocs, pthread_t * threads);
