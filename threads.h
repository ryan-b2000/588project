#include "datatypes.h"


//======================================================================//
typedef struct {
	int id;									// the thread ID
	int start;							// start of partition
	int len;								// length of partition
	EMPLOYEE * employees;		// pointer to master table
	TRIP * trips;						// pointer to master table
} THREAD_ARGS;



//======================================================================//

void* thread_sort_employees(void* input);

void* thread_sort_trips(void* input);

EMPLOYEE * read_employee_table(EMPLOYEE * table, int start, int len);

TRIP * read_trips_table(TRIP * table, int start, int len);

void sort_employee(EMPLOYEE * part, int len, int id);

void print_thread_info(THREAD_ARGS args);

void swap_employee(EMPLOYEE * dest, EMPLOYEE * src);
