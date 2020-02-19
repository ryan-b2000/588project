#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "fileio.h"
#include "table.h"
#include "threads.h"
#include "datatypes.h"

//======================================================================//

int numprocs;

ENTRY * employee_table;		// table of employee names read in from file
ENTRY * trips_table;			// table of trips read in from file
PARTITION * partitions;		// structure of partition and sub-partition statistics
MERGED_ENTRIES * merged;	// final table containing merged entries


//======================================================================//
int set_processors(int argc, char* argv[]);
void write_time_to_file(unsigned long long int nanos, unsigned long long int secs);


//======================================================================//


int main (int argc, char *argv[]) {

	int i, ret;
	struct timespec   StartTime;
	struct timespec   EndTime;

	ret = clock_gettime(CLOCK_REALTIME, &StartTime);

	// check arguments and set number of processors
	numprocs = set_processors(argc, argv);

	// load employee and trips tables
	if (file_load_employees_table(&employee_table) < 0)
		return -1;
	if (file_load_trips_table(&trips_table) < 0)
		return -1;

	// allocate memory for the partition and sub-partition information
	partitions = (PARTITION*) malloc(sizeof(PARTITION) * numprocs);
	for (i = 0; i < numprocs; i++) {
		partitions[i].subpart = (SUBPART*) malloc(sizeof(SUBPART) * numprocs);
	}

	// allocate memory for the final merged entries table
	merged = (MERGED_ENTRIES*) malloc(sizeof(MERGED_ENTRIES) * MAX_EMPLOYEES);

	// partition employee table, sort the partitions, and consolidate
	split_and_sort_tables();

	ret = clock_gettime(CLOCK_REALTIME, &EndTime);
	unsigned long long int nanos = ((EndTime.tv_sec - StartTime.tv_sec) * 1000000000) + EndTime.tv_nsec - StartTime.tv_nsec;
	unsigned long long int secs = (EndTime.tv_sec - StartTime.tv_sec) + (EndTime.tv_nsec - StartTime.tv_nsec) / 1000000000;

	printf("Time = %lld nanoseconds (%.8f sec)\n", nanos , (double)((double)nanos / 1000000000));

	write_merged_table_to_file("merged.txt");

	write_time_to_file(nanos, secs);

	free (partitions);
	free (merged);
	printf("Finished Run %d\n", numprocs);

	return 0;
}


/*************************************************************************************
 *
 */
void write_time_to_file(unsigned long long int nanos, unsigned long long int secs) {

	FILE * file;

	file = fopen("times.txt", "a");
	if (file == NULL) {
		printf("Error opening file.\n");
	}

	fprintf(file, "%d \t %.8f \t %lld\n", numprocs, (double)((double)nanos / 1000000000), nanos);

	fclose(file);
}


/**************************************************************************************
 * 	Set the number of processors
 */
int set_processors(int argc, char* argv[]) {

	int numprocs;

	if (argc < 2)
		numprocs = 1;
	else 
		numprocs = atoi(argv[1]);

	printf("Numprocs: %d\n", numprocs);

	return numprocs;
}
