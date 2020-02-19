#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

	// partition employee table, sort the partitions, and consolidate
	split_and_sort_tables();

	write_merged_table_to_file("merged.txt");

#if 0
	printf("Finished\n");
#endif
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

#if 0
	printf("Numprocs: %d\n", numprocs);
#endif
	return numprocs;
}
