#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "fileio.h"
#include "table.h"
#include "threads.h"
#include "datatypes.h"

#define DEBUG 1


//======================================================================//



EMPLOYEE * employee_table;
TRIP * trips_table;



//======================================================================//
int set_processors(int argc, char* argv[]);


//======================================================================//


int main (int argc, char *argv[]) {

	int i, ret, psize, numprocs;


	// check arguments and set number of processors
	numprocs = set_processors(argc, argv);

	// set partition size
	psize = MAX_EMPLOYEES / numprocs;

	// load employee and trips tables
	if (file_load_employees_table(&employee_table) < 0)
		return -1;
	if (file_load_trips_table(&trips_table) < 0)
		return -1;

	// partition employee table and sort the partitions
	split_and_sort_employees(&employee_table, numprocs);
	merge_sorted_employees(&employee_table, numprocs);

	// partition trips table and sort the partitions
	split_and_sort_trips(&trips_table, numprocs);
	merge_sorted_trips(&trips_table, numprocs);

	// merge-join employees and trips tables
	merge_join_tables(&employee_table, &trips_table, numprocs);


	printf("Finished\n");
	return 0;
}






/**************************************************************************************
 * 	Barrier to stop threads until all have finished
 */
void thread_barrier(int numprocs, pthread_t * threads) {

	int i, ret;

	// wait for worker threads to finish
	for (i = 0; i < numprocs; i++) {
		#if DEBUG
			printf("Waiting for thread: %d\n", i);
		#endif
		ret = pthread_join(threads[i], NULL);
	}
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
