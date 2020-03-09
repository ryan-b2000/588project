/*		Ryan Bentz, Ignacio Roberto Genovese, Rezwana Mahbub
 * 		ECE 588 Final Project
 * 		2020/03/05
 */

#include <pthread.h>
#include "datatypes.h"


//======================================================================//


//======================================================================//

void* thread_worker(void* input);


int set_partition_len(int id, int numprocs, int max);

ENTRY* get_partition(ENTRY * table, int start, int len);


void sort_partition(ENTRY * part, int len, int id);


void print_thread_info(const char * type, int id, int start, int len);


void swap_entries(ENTRY * dest, ENTRY * src);


void thread_barrier (int numprocs, pthread_t * threads);
