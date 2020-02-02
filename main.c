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

int numprocs;

EMPLOYEE * employee_table;
TRIP * trips_table;



//======================================================================//
void set_processors(int argc, char* argv[]);


//======================================================================//


int main (int argc, char *argv[]) {

	int i, ret, psize;
	pthread_attr_t attr;
	pthread_t * threads;
	THREAD_ARGS * args;

	// check arguments and set number of processors
	set_processors(argc, argv);

	// set partition size
	psize = MAX_EMPLOYEES / numprocs;

	// load employee and trips tables
	if (file_load_employees_table(&employee_table) < 0)
		return -1;
	if (file_load_trips_table(&trips_table) < 0)
		return -1;

	// set thread attibutes
	pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  // allocate space for threads and args
	threads = (pthread_t*) malloc(sizeof(pthread_t) * numprocs);
	args = (THREAD_ARGS*) malloc (sizeof(THREAD_ARGS) * numprocs);

	// start the worker threads
	for (i = 0; i < numprocs; i++) {
		// set the thread args
		args[i].id = i;
		args[i].start = i * psize;
		args[i].len = psize;
		args[i].trips = trips_table;
		args[i].employees = employee_table;
		ret = pthread_create(&threads[i], &attr, thread_function, (void*)&args[i]);
	}

	// wait for worker threads to finish
	for (i = 0; i < numprocs; i++) {
		#if DEBUG
			printf("Waiting for thread: %d\n", i);
		#endif
		ret = pthread_join(threads[i], NULL);
	}

	printf("Finished\n");
	return 0;
}


void set_processors(int argc, char* argv[]) {

	if (argc < 2)
		numprocs = 1;
	else 
		numprocs = atoi(argv[1]);

	#if DEBUG
	  	printf("Numprocs: %d\n", numprocs);
	#endif

	return;
}
