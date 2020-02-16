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

ENTRY * employee_table;
ENTRY * trips_table;




//======================================================================//
int set_processors(int argc, char* argv[]);


//======================================================================//


int main (int argc, char *argv[]) {

	int i, ret, psize;


	// check arguments and set number of processors
	numprocs = set_processors(argc, argv);

	// load employee and trips tables
	if (file_load_employees_table(&employee_table) < 0)
		return -1;
	if (file_load_trips_table(&trips_table) < 0)
		return -1;

	// partition employee table, sort the partitions, and consolidate
	split_and_sort_table(employee_table, TABLE_EMPLOYEES);
	merge_sorted_partitions(employee_table, TABLE_EMPLOYEES);
	write_table_to_file(employee_table, "sorted_emps.txt", TABLE_EMPLOYEES);

	// partition trips table, sort the partitions, and consolidate
	split_and_sort_table(trips_table, TABLE_TRIPS);
	merge_sorted_partitions(trips_table, TABLE_TRIPS);
	write_table_to_file(trips_table, "sorted_trips.txt", TABLE_TRIPS);

	// merge-join employees and trips tables
	//merge_join_tables(&employee_table, &trips_table);


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
