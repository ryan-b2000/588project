#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "threads.h"
#include "datatypes.h"

//======================================================================//
// 1 = TURN ON, 0 = TURN OFF
#define DEBUG_LOW 1
#define DEBUG_HIGH 0


//======================================================================//
//extern ENTRY ** partitions;
extern PARTITION * partitions;

/**************************************************************************************
 * 	Main sorting thread
 */
void* thread_sort_partition(void* input) {
	
	int i, id, start, len;
	ENTRY * partition;
	ENTRY * table;

	// parse the thread args
	id = ((THREAD_ARGS*)input)->id;
	start = ((THREAD_ARGS*)input)->start;
	len = ((THREAD_ARGS*)input)->len;
	table = ((THREAD_ARGS*)input)->table;

	print_thread_info(*(THREAD_ARGS*)input);

	// read the table and make a copy of a partition
	partition = get_partition(table, start, len);

	// sort the partition
	sort_partition(partition, len, id);

	partitions[id].entry = partition;

	return NULL;
}



/*************************************************************************************
 * Copy a partition of the ENTRY table
 *
 * NOTE: We are also going to clear the original table entry because we have it in cache and we will override it later.
 * Returns a pointer to the new partition
 */
ENTRY * get_partition(ENTRY * table, int start, int len) {

	int i, j, id;
	ENTRY * partition;

	// allocate memory for table partition
	partition = (ENTRY*) malloc(sizeof(ENTRY) * len);

	j = 0;
	for (i = start; i < (start + len); i++) {
		// copy the table entry
		partition[j].id = table[i].id;
		strncpy(partition[j].desc, table[i].desc, SIZE_DESC);

		if (table[i].id == 0) {
			printf("Table entry is NULL\n");
		}

		j++;
	}

	return partition;
}


/*************************************************************************************
 * Sort the partition
 * part :: pointer to the start of the partition in the table
 * len	:: length of the partition
 * id		:: thread ID
 */
void sort_partition(ENTRY * part, int len, int threadID) {

	int i;

	bool sorted = false;

#if DEBUG_HIGH
	printf("Presort:\n");
	for(i = 0; i < len; i++) {
		printf("  %d %s\n", entry[i].id, entry[i].desc);
	}
#endif

	// simple swap sort
	while (sorted == false) {
		sorted = true;
		for (i = 1; i < len; i++) {
			if (part[i].id < part[i-1].id) {
				swap_entries(&part[i], &part[i-1]);
				sorted = false;
			}
		}
	}

#if DEBUG_HIGH
	printf("Endsort:\n");
		for(i = 0; i < len; i++) {
			printf("  %d: %d %s\n", threadID, entry[i].id, entry[i].desc);
		}
#endif
}


/*************************************************************************************
 * Swap two ENTRY entries
 *
 */
void swap_entries(ENTRY * dest, ENTRY * src) {
	ENTRY temp;

	temp.id = dest->id;	// desc -> temp
	dest->id = src->id; // src -> desc
	src->id = temp.id;	// temp -> src

	strncpy(temp.desc, dest->desc, SIZE_DESC);
	strncpy(dest->desc, src->desc, SIZE_DESC);
	strncpy(src->desc, temp.desc, SIZE_DESC);
}




/**************************************************************************************
 * 	Barrier to stop threads until all have finished
 */
void thread_barrier(int numprocs, pthread_t * threads) {

	int i, ret;
	int * id;

	// wait for worker threads to finish
	for (i = 0; i < numprocs; i++) {
		#if DEBUG_LOW
			printf("Waiting for thread: %d\n", i);
		#endif
		ret = pthread_join(threads[i], NULL);
	}
}


/**************************************************************************************
 * Print out thread info for DEBUG_LEVELging
 */
void print_thread_info(THREAD_ARGS args) {
	printf("Thread %d: Start: %d End: %d Len: %d\n", args.id, args.start, args.start + args.len - 1, args.len);
}
