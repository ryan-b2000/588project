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
extern PARTITION * partitions;
extern ENTRY * employee_table;
extern ENTRY * trips_table;
extern MERGED_ENTRIES * merged;
pthread_t * threads;



/**************************************************************************************
 *
 */
void init_subparts(int id, ENTRY * table, int tabletype) {

	int i, test;
	int part_start, part_len, part_num;
	int sub_start, sub_len, subrange;
	int key;
	PARTITION * part;

	// set the generic table, partition start, and length based on table type
	if (tabletype == TABLE_EMPLOYEES) {
		table = employee_table;
		part_start = id * (MAX_EMPLOYEES / numprocs);
		part_len = MAX_EMPLOYEES / numprocs;
	}
	else {
		table = trips_table;
		part_start = id * (MAX_TRIPS / numprocs);
		part_len = MAX_TRIPS / numprocs;
	}

	// set the master partition params
	part = &partitions[id];											// assign to the partition we are interested in
	part->start_ptr = (table + part_start);			// set the pointer to the first item in the partition
	part->start = part_start;										// set the master starting index of the partition
	part->len = part_len;												// set the overall size of the partition

	// init the sub-partition params
	part_num = 0;
	sub_start = 0;
	sub_len = 0;
	// set key to demarcation point of the first sub-partition.
	// If sub-partition 0 then we want all entries from 0 to max / numprocs
	key = (part_num + 1) * (MAX_ENTRIES / numprocs);

	// go through the partition and find the demarcation points of the sub-partitions based on key values
	for (i = 0; i < part_len; i++) {

		// increment the length counter on each iteration
		sub_len++;

		// if the test value exceeds the key value then we have reached the demarcation point
		test = (part->start_ptr + i)->id;
		if (test >= key) {

			// set the sub partition parameters
			part->subpart[part_num].start_ptr = part->start_ptr + sub_start;
			part->subpart[part_num].len = sub_len;
			part->subpart[part_num].start = sub_start;

			//if (id == 0)	printf("--- ID= %2d, SubStart= %4d = %d %s\n", id, sub_start, part->subpart[part_num].start_ptr[0].id, part->subpart[part_num].start_ptr[0].desc);

			// set the next key to look for
			part_num++;
			key = (part_num + 1) * (MAX_ENTRIES / numprocs);

			// update the substart position
			sub_start = i;
			// reset the sub-partition length
			sub_len = 0;
		}
	}

	// do the last partition
	sub_len = part_len - sub_start;
	part->subpart[part_num].start_ptr = part->start_ptr + sub_start;
	part->subpart[part_num].len = sub_len;
	part->subpart[part_num].start = sub_start;

}


/**************************************************************************************
 *
 */
void verify_subparts(int id, ENTRY * table, int tabletype) {

	int i, total = 0;
	PARTITION * part;
	SUBPART * subpart;

	if (id == 0) {
		for (i = 0; i < numprocs; i++) {
			part = &partitions[id];
			subpart = &part->subpart[i];

			total += subpart->len;

			printf("Verify Subparts -- Part %2d Sub %2d: Start= %4d, Len= %4d", id, i, subpart->start, subpart->len);
			printf(" -- %d %s\n", subpart->start_ptr[0].id, subpart->start_ptr[0].desc);

		}
		printf("Verify Subparts -- Part %2d: Partlen %5d, Total Sublen= %5d\n", id, part->len, total);
	}
}


/**************************************************************************************
 *
 */
void split_and_sort_tables() {

	int i, psize, ret;
	pthread_attr_t attr;
	THREAD_ARGS * args;

	printf("\n----------------\nSplit and sort table partitions...\n");

	// set thread attibutes
	pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  // allocate space for threads and args
	threads = (pthread_t*) malloc(sizeof(pthread_t) * numprocs);
	args = (THREAD_ARGS*) malloc (sizeof(THREAD_ARGS) * numprocs);

	// start the worker threads
	for (i = 0; i < numprocs; i++) {
		args[i].id = i;
		ret = pthread_create(&threads[i], &attr, thread_sort_partition, (void*)&args[i]);
	}

	// make sure all threads have returned
	for (i = 0; i < numprocs; i++) {
		if (pthread_join(threads[i], NULL) < 0)
			printf("Error joining thread %d\n", i);
	}

	// reclaim memory
	free(threads);
	free(args);
}



/*************************************************************************************
 *
 */
void merge_join(int id, ENTRY * table, int tabletype) {

	int key, i, p;
	int lowest, part, temp, count;
	int substart, sublen;
	int start, len;
	int mlen, mstart;

	if (tabletype == TABLE_EMPLOYEES) {
		// set the starting key number
		start = (MAX_EMPLOYEES / numprocs) * id;
		len = (MAX_EMPLOYEES / numprocs);

		// iterate through each entry in the merged-join table assigned to this thread
		for (key = start; key < (start + len); key++) {

			// scan through each partition subsection and count matching key-value matches
			count = count_key_matches(id, key);

			if (count > 0) {
	#if 0
				printf("ID: %d -- Found %4d names for key %5d\n", id, count, key);
	#endif
				// allocate memory for the key value pairs and merge entries
				merged[key].employees.desc = (char**) malloc(sizeof(char*) * count);
				for (i = 0; i < count; i++)
					merged[key].employees.desc[i] = (char*) malloc(sizeof(char) * SIZE_DESC);

				// copy the input entries to the sorted table entry
				merged[key].employees.count = count;
				//merge_employee_entries(key, id);
				merge_entries(key, id, &merged[key], TABLE_EMPLOYEES);
			}
			else {
				merged[key].employees.desc = NULL;
			}
		}
	}
	else {
		// set the starting key number
		start = (MAX_EMPLOYEES / numprocs) * id;
		len = (MAX_EMPLOYEES / numprocs);

		// iterate through each entry in the merged-join table assigned to this thread
		for (key = start; key < (start + len); key++) {

			// scan through each partition subsection and count matching key-value matches
			count = count_key_matches(id, key);


			if (count > 0) {
	#if 1
				if (key == 19267)
					printf("ID: %d -- Found %4d cities for key %5d\n", id, count, key);
	#endif
				// allocate memory for the key value pairs
				merged[key].trips.desc = (char**) malloc(sizeof(char*) * count);
				for (i = 0; i < count; i++)
					merged[key].trips.desc[i] = (char*) malloc(sizeof(char) * SIZE_DESC);

				// copy the input entries to the sorted table entry
				merged[key].trips.count = count;
				//merge_trip_entries(key, id);
				merge_entries(key, id, &merged[key], TABLE_TRIPS);
			}
			else {
				merged[key].trips.desc = NULL;
			}
		}
	}
}


/*************************************************************************************
 *
 */
void merge_entries(int key, int id, MERGED_ENTRIES * entry, int type) {

	int i, p;
	int count = 0;
	int test, len;
	ENTRY * substart;
	SUBPART * subpart;


	for (p = 0; p < numprocs; p++) {

		subpart = &partitions[p].subpart[id];
		substart = subpart->start_ptr;
		len = subpart->len;

		// iterate through the sub-partition
		for (i = 0; i < len; i++) {

			test = (*(substart + i)).id;

			if (key == test) {
				if (type == TABLE_EMPLOYEES)
					strncpy(entry->employees.desc[count], (*(substart + i)).desc, SIZE_DESC);
				else
					strncpy(entry->trips.desc[count], (*(substart + i)).desc, SIZE_DESC);
				count++;
			}

			// if we are greater than the key value, we are done counting
			if (key < test)
				break;
		}
	}
}


/*************************************************************************************
 *
 */
void merge_employee_entries(int key, int id) {

	int i, p;
	int count = 0;
	int test, len;
	ENTRY * substart;
	SUBPART * subpart;

	//

	for (p = 0; p < numprocs; p++) {

		subpart = &partitions[p].subpart[id];
		substart = subpart->start_ptr;
		len = subpart->len;

		// iterate through the sub-partition
		for (i = 0; i < len; i++) {

			test = (*(substart + i)).id;

			if (key == test) {
				strncpy(merged[key].employees.desc[count], (*(substart + i)).desc, SIZE_DESC);
				count++;
			}

			// if we are greater than the key value, we are done counting
			if (key < test)
				break;
		}
	}
}


/*************************************************************************************
 *
 */
void merge_trip_entries(int key, int id) {

	int i, p;
	int count = 0;
	int test, len;
	ENTRY * substart;
	SUBPART * subpart;

	//

	for (p = 0; p < numprocs; p++) {

		subpart = &partitions[p].subpart[id];
		substart = subpart->start_ptr;
		len = subpart->len;

		// iterate through the sub-partition
		for (i = 0; i < len; i++) {

			test = (*(substart + i)).id;

			if (key == test) {
				strncpy(merged[key].trips.desc[count], (*(substart + i)).desc, SIZE_DESC);
				count++;
			}

			// if we are greater than the key value, we are done counting
			if (key < test)
				break;
		}
	}

}


/*************************************************************************************
 *
 */
int count_key_matches(int id, int key) {

	int i, p;
	int count = 0;
	int test, len;
	ENTRY * substart;
	SUBPART * subpart;

	for (p = 0; p < numprocs; p++) {

		subpart = &partitions[p].subpart[id];
		substart = subpart->start_ptr;
		len = subpart->len;

		// iterate through the sub-partition
		for (i = 0; i < len; i++) {

			test = (*(substart + i)).id;
			// count number of matching keys
			if (key == test)
				count++;

			// if we are greater than the key value, we are done counting
			if (key < test)
				break;
		}
	}

	return count;
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
			//temp = get_employee_number(partitions[p]);
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
			//temp = get_employee_number(partitions[p]);
			if (temp == lowest) {
				//ret = add_sorted_entry(&partitions[p], table, &master_index);
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
int get_next_key(PARTITION partition) {
	//int index = partition.index;
	//return partition.entry[index].id;
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
		//printf("%d - %d %s\n", i, partition.entry[i].id, partition.entry[i].desc);
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


/**************************************************************************************
 *
 */
void assign_key_ranges(int * range) {

	int i, sublen;

	sublen = MAX_EMPLOYEES / (numprocs * 2);

	for (i = 0; i < numprocs; i++) {
		range[i] = i * sublen;
		printf("Key Range %d: High = %d\n", i, range[i]);
	}

}

