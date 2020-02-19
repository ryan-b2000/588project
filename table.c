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


extern PARTITION * partitions;
extern ENTRY * employee_table;
extern ENTRY * trips_table;
extern MERGED_ENTRIES * merged;
pthread_t * threads;

pthread_mutex_t lock_verify_parts;

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
	//part->start_ptr = (table + part_start);			// set the pointer to the first item in the partition
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
		test = (table + part_start + i)->id;
		//test = (part->start_ptr + i)->id;

		if (test >= key) {

			// set the sub partition parameters
			//part->subpart[part_num].start_ptr = part->start_ptr + sub_start;
			part->subpart[part_num].len = sub_len;
			part->subpart[part_num].start = sub_start;

			//printf("Set Partition %d Subpart %d -- Substart ID= %4d, Subptr= %p\n", id, part_num, sub_start, &part->subpart[part_num].start_ptr);
			//if (id == 0)	printf("--- ID= %2d, SubStart= %4d = %d %s\n", id, sub_start, part->subpart[part_num].start_ptr[0].id, part->subpart[part_num].start_ptr[0].desc);

			// set the next key to look for
			part_num++;
			key = (part_num + 1) * (MAX_ENTRIES / numprocs);

			// update the start position of the next sub-partition
			sub_start = i;
			// reset the sub-partition length
			sub_len = 0;
		}
	}

	// get the statistics for the last sub-partition
	sub_len = part_len - sub_start;
	//part->subpart[part_num].start_ptr = part->start_ptr + sub_start;
	part->subpart[part_num].len = sub_len;
	part->subpart[part_num].start = sub_start;
	//printf("Set Partition %d Subpart %d -- Substart ID= %4d\n", id, part_num, sub_start);
}


/**************************************************************************************
 *
 */
void verify_subparts(int id, ENTRY * table, int tabletype) {

	int i, total = 0;
	PARTITION * part;
	SUBPART * subpart;
	int index;

#if 0
	pthread_mutex_lock(&lock_verify_parts);

	for (i = 0; i < numprocs; i++) {
		part = &partitions[id];
		subpart = &part->subpart[i];

		total += subpart->len;

		printf("Verify Subparts -- Part %2d Sub %2d: Start= %4d, Len= %4d\n", id, i, subpart->start, subpart->len);

	}
	printf("Verify Subparts -- Part %2d: Partlen %5d, Total Sublen= %5d\n", id, part->len, total);

	pthread_mutex_unlock(&lock_verify_parts);
#endif
}


/**************************************************************************************
 *
 */
void split_and_sort_tables() {

	int i, psize, ret;
	pthread_attr_t attr;
	THREAD_ARGS * args;

#if 0
	printf("\n--  Split and sort table partitions...\n");
#endif

	// set thread attibutes
	pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  // allocate space for threads and args
	threads = (pthread_t*) malloc(sizeof(pthread_t) * numprocs);
	args = (THREAD_ARGS*) malloc (sizeof(THREAD_ARGS) * numprocs);

	// start the worker threads
	for (i = 0; i < numprocs; i++) {
		args[i].id = i;
		ret = pthread_create(&threads[i], &attr, thread_worker, (void*)&args[i]);
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
			count = count_key_matches(id, key, employee_table);

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
			count = count_key_matches(id, key, trips_table);


			if (count > 0) {
	#if 0
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
	int test, len, index;
	ENTRY * substart;			// start of the sub-partition
	SUBPART * subpart;		// pointer to the sub-partition stats structure


	if (type == TABLE_EMPLOYEES) {
		for (p = 0; p < numprocs; p++) {

			subpart = &partitions[p].subpart[id];		// point to sub-partition stats based on our thread
			substart = subpart->start_ptr;					// get the sub-partition stats
			len = subpart->len;

			// iterate through the sub-partition
			for (i = 0; i < len; i++) {

				index = partitions[p].start + partitions[p].subpart[id].start + i;
				test = employee_table[index].id;

				if (key == test) {
						if (count < entry->employees.count)
							strncpy(entry->employees.desc[count], employee_table[index].desc, SIZE_DESC);
						else
							printf("EMPLS: Error trying to add entry! Partition %d SubPart %d - Employee ID %d\n", id, p, test);
					count++;
				}

				// if we are greater than the key value, we are done counting
				if (key < test)
					break;
			}
		}
	}
	// MERGE TRIPS ENTRIES
	else {
		for (p = 0; p < numprocs; p++) {

			subpart = &partitions[p].subpart[id];
			substart = subpart->start_ptr;
			len = subpart->len;

			// iterate through the sub-partition
			for (i = 0; i < len; i++) {

				index = partitions[p].start + partitions[p].subpart[id].start + i;
				test = trips_table[index].id;

				if (key == test) {
					if (count < entry->trips.count)
						strncpy(entry->trips.desc[count], trips_table[index].desc, SIZE_DESC);
					else
						printf("TRIPS: Error trying to add entry! Partition %d SubPart %d - Employee ID %d\n", id, p, test);
					count++;
				}

				// if we are greater than the key value, we are done counting
				if (key < test)
					break;
			}
		}
	}
}


/*************************************************************************************
 *
 */
int count_key_matches(int id, int key, ENTRY * table) {

	int i, p;
	int count = 0;
	int test, len, index;
	ENTRY * substart;
	SUBPART * subpart;

	for (p = 0; p < numprocs; p++) {

		subpart = &partitions[p].subpart[id];
		substart = subpart->start_ptr;
		len = subpart->len;

		// iterate through the sub-partition
		for (i = 0; i < len; i++) {

			index = partitions[p].start + partitions[p].subpart[id].start + i;
			test = table[index].id;

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

