#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "threads.h"
#include "fileio.h"
#include "table.h"
#include "datatypes.h"


//======================================================================//
extern int numprocs;

//ENTRY ** partitions;		// array of pointers to the sorted partitions before merging
PARTITION * partitions;


/**************************************************************************************
 *
 */
void split_and_sort_table(ENTRY * table, int tabletype) {

	int i, psize, ret;
	pthread_attr_t attr;
	pthread_t * threads;
	THREAD_ARGS * args;

	// set thread attibutes
	pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  if (tabletype == TABLE_TRIPS) {
  	printf("\n----------------\nSplit and sort trip table partitions...\n");
  	psize = MAX_TRIPS / numprocs;
  }

  else {
  	printf("\n----------------\nSplit and sort employee table partitions...\n");
  	psize = MAX_EMPLOYEES / numprocs;
  }

#if 0
  print_table(table, tabletype);
#endif

  // allocate space for threads and args
	threads = (pthread_t*) malloc(sizeof(pthread_t) * numprocs);
	args = (THREAD_ARGS*) malloc (sizeof(THREAD_ARGS) * numprocs);

	// allocate memory for sorted partition pointers
	partitions = (PARTITION*) malloc(sizeof(PARTITION) * numprocs);
	for (i = 0; i < numprocs; i++) {
		partitions[i].index = 0;
		partitions[i].len = psize;
	}

	// start the worker threads
	for (i = 0; i < numprocs; i++) {
		// set the thread args
		args[i].id = i;
		args[i].start = i * psize;
		args[i].len = psize;
		args[i].table = table;
		ret = pthread_create(&threads[i], &attr, thread_sort_partition, (void*)&args[i]);
	}

	// wait for threads to finish and get the array of partition pointers
	thread_barrier(numprocs, threads);

	// print the sorted partitions one at a time
#if 0
	for (i = 0; i < numprocs; i++) {
		//print_partition(partitions[i], i, psize);
		//printf("\n");
		write_partition_to_file(partitions[i], tabletype, i, psize);
	}
#endif

	// reclaim memory
	free(threads);
	free(args);
}


/**************************************************************************************
 *
 */
void merge_sorted_partitions (ENTRY * table, int tabletype) {

	int i, p, part, tsize, ret;
	int temp, lowest;
	int master_index = 0;


	if (tabletype == TABLE_TRIPS) {
		tsize = MAX_TRIPS;
		printf("Merge trips partitions...\n");
	}
	else {
		tsize = MAX_EMPLOYEES;
		printf("Merge employees partitions...\n");
	}

#if 0
	for (i = 0; i < numprocs; i++) {
		write_partition_to_file(partitions[i], tabletype, i);
	}
#endif


	// for each master table entry, find the lowest next employee number then copy all matching to master table array
	for (i = 0; i < tsize; i++) {

		// find the lowest employee number for all next entries
		lowest = MAX_TRIPS + MAX_EMPLOYEES;
		part = 0;
		for (p = 0; p < numprocs; p++) {
			temp = get_employee_number(partitions[p]);
			if (temp < lowest && temp != 0) {		// check for temp != 0 because EOF partition entry messes it up
				lowest = temp;
				part = p;
			}
#if 0
			// if the entry is zero, there is an error in the partition so increment the index to get past it
			if (temp == 0) {
				if (partitions[p].index < partitions[p].len)
					partitions[p].index++;
			}
#endif
		}

		// cycle through the partitions and add any entries that match the next lowest employee number
		for (p = 0; p < numprocs; p++) {
			temp = get_employee_number(partitions[p]);
			if (temp == lowest) {
				ret = add_sorted_entry(&partitions[p], table, &master_index);
				if (ret < 0) {
					printf("Error: partition maxed out.\n");
				}
			}
		}
	}
}


/**************************************************************************************
 *
 */
int get_employee_number(PARTITION partition) {
	int index = partition.index;
	return partition.entry[index].id;
}


/**************************************************************************************
 *
 */
int add_sorted_entry(PARTITION * partition, ENTRY * table, int * index) {

	int ti = *index;
	int pi = partition->index;
	int i;

	// check if this partition is maxed out
	if (pi == partition->len) {
		return -1;
	}

#if 0
	printf("Override master entry %6d: %d %s with %d %s\n", ti, table[ti].id, table[ti].desc, partition->entry[pi].id, partition->entry[pi].desc);
#endif

	// copy members
	table[ti].id = partition->entry[pi].id;
	memcpy(table[ti].desc, partition->entry[pi].desc, SIZE_DESC);

	// increment master table index and partition local index
	(partition->index)++;
	(*index)++;

	return 0;
}


/**************************************************************************************
 *
 */
void merge_join_tables(ENTRY ** etable, ENTRY ** ttable) {

}


/**************************************************************************************
 *
 */
void print_partition(PARTITION partition, int id) {

	int i;

	printf("Printing sorted partition %d...\n", id);
	for (i = 0; i < 25; i++) {
		printf("%d - %d %s\n", i, partition.entry[i].id, partition.entry[i].desc);
	}
	printf("\n");
}


/**************************************************************************************
 *
 */
void print_table(ENTRY * table, int tabletype) {
	int i, tsize;

	if (tabletype == TABLE_TRIPS) {
		tsize = MAX_TRIPS;
		printf("Print trips table:\n");
	}
	else {
		tsize = MAX_EMPLOYEES;
		printf("Print employees table:\n");
	}

	for (i = 0; i < tsize; i++) {
		printf("%6d %d %s\n", i, table[i].id, table[i].desc);
	}

}
