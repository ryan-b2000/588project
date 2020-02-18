#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fileio.h"
#include "table.h"
#include "threads.h"
#include "datatypes.h"

#define DEBUG 1


//======================================================================//

int numprocs;
int * key_range;
ENTRY * employee_table;
ENTRY * trips_table;
PARTITION * partitions;
HISTOGRAM ** histogram;
MERGED_ENTRIES * merged;

//======================================================================//
int set_processors(int argc, char* argv[]);



//======================================================================//


int main (int argc, char *argv[]) {

	int i;

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

	// allocate memory for the key range structure
	//key_range = (int*) malloc(sizeof(int) * numprocs);

	// assign generic key ranges independent of distribution
	//assign_key_ranges(key_range);

	// partition employee table, sort the partitions, and consolidate
	split_and_sort_tables();

	write_merged_table_to_file("merged.txt");
	//write_table_to_file(employee_table, "sorted_emps.txt", TABLE_EMPLOYEES);
	//write_table_to_file(trips_table, "sorted_trips.txt", TABLE_TRIPS);



	printf("Finished\n");
	return 0;
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

	#if DEBUG
	  	printf("Numprocs: %d\n", numprocs);
	#endif

	return numprocs;
}
