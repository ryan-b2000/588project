#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "table.h"
#include "threads.h"
#include "fileio.h"
#include "datatypes.h"

//======================================================================//
// 1 = TURN ON, 0 = TURN OFF
#define DEBUG_LOW 1
#define DEBUG_HIGH 0


//======================================================================//
extern int numprocs;
extern PARTITION * partitions;
extern ENTRY * employee_table;
extern ENTRY * trips_table;

extern pthread_t * threads;

int sync_count = 0;
pthread_cond_t sync_cv;
pthread_mutex_t sync_lock;



/**************************************************************************************
 * 	Main sorting thread
 */
void* thread_sort_partition(void* input) {
	
	int i, id, start, len;
	ENTRY * table;

	id = ((THREAD_ARGS*)input)->id;

	// EMPLOYEES
	//-----------------------------------------------------------------
	start = id * (MAX_EMPLOYEES / numprocs);
	len = MAX_EMPLOYEES / numprocs;
	table = employee_table;

	print_thread_info("Sorting Employees", id, start, len);

	// sort the table partition assigned to this thread
	sort_partition((table + start), len, id);

	// DEBUG: write partition to file
	write_partition_to_file(employee_table, TABLE_EMPLOYEES, id);

	// wait for sorting to be complete
	thread_barrier(numprocs, threads);
	if (id == 0)	printf("\n --- Partition sorting complete ---\n\n");

	// analyze the sorted partitions and create sub-partitions based on key ranges
	init_subparts(id, table, TABLE_EMPLOYEES);

	// DEBUG: print out sorted partition states
	verify_subparts(id, table, TABLE_EMPLOYEES);

	thread_barrier(numprocs, threads);
	if (id == 0)	printf("\n --- Performing Merge Join ---\n\n");

	// Copy the members from the assigned partition into the merged join table
	merge_join(id, table, TABLE_EMPLOYEES);


	// TRIPS
	//-----------------------------------------------------------------
	start = id * (MAX_TRIPS / numprocs);
	len = MAX_TRIPS / numprocs;
	table = trips_table;

	print_thread_info("Sorting Trips", id, start, len);

	// sort the table partition assigned to this thread
	sort_partition((table + start), len, id);

	// DEBUG: write partition to file
	write_partition_to_file(trips_table, TABLE_TRIPS, id);

	thread_barrier(numprocs, threads);
	if (id == 0)	printf("\n --- Partition sorting complete ---\n\n");

	// analyze the sorted partitions and create sub-partitions based on key ranges
	init_subparts(id, table, TABLE_TRIPS);

	// DEBUG: print out sorted partition states
	verify_subparts(id, table, TABLE_TRIPS);

	thread_barrier(numprocs, threads);
	if (id == 0)	printf("\n --- Performing Merge Join ---\n\n");

	// Copy the members from the assigned partition into the merged join table
	merge_join(id, table, TABLE_TRIPS);

	//thread_barrier(numprocs, threads);

	printf("Thread %d -- finished\n", id);
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

#if 0
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

#if 0
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

	int ret;

	pthread_mutex_lock(&sync_lock);
	sync_count++;
	// all threads reach barrier when it a multiple of numprocs
	if (sync_count % numprocs == 0)
		ret = pthread_cond_broadcast(&sync_cv);
	else
		ret = pthread_cond_wait(&sync_cv, &sync_lock);
	pthread_mutex_unlock(&sync_lock);
}


/**************************************************************************************
 * Print out thread info for DEBUG_LEVELging
 */
void print_thread_info(const char * type, int id, int start, int len) {
	printf("Thread %d: %s Table -- Start= %6d End= %6d Len= %6d\n", id, type, start, (start + len - 1), len);
}
