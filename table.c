#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "table.h"
#include "datatypes.h"



/**************************************************************************************
 *
 */
void split_and_sort_employees(EMPLOYEE * table, int numprocs) {

	int i, psize, ret;
	pthread_attr_t attr;
	pthread_t * threads;
	THREAD_ARGS * args;

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
		args[i].trips = NULL;
		args[i].employees = employee_table;
		ret = pthread_create(&threads[i], &attr, thread_sort_employees, (void*)&args[i]);
	}

	free(threads);
	free(args);
}


/**************************************************************************************
 *
 */
void split_and_sort_trips(TRIP * table, int numprocs) {

	int i, psize, ret;
	pthread_attr_t attr;
	pthread_t * threads;
	THREAD_ARGS * args;

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
		args[i].employees = NULL;
		ret = pthread_create(&threads[i], &attr, thread_sort_trips, (void*)&args[i]);
	}

	free(threads);
	free(args);
}
