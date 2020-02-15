#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "threads.h"
#include "datatypes.h"

#define DEBUG 0


/**************************************************************************************
 * 	Main sorting thread
 */
void* thread_sort_employees(void* input) {
	
	int i, id, start, len;
	EMPLOYEE * e_table;
	TRIP * t_table;

	// parse the thread args
	id = ((THREAD_ARGS*)input)->id;
	start = ((THREAD_ARGS*)input)->start;
	len = ((THREAD_ARGS*)input)->len;
	e_table = ((THREAD_ARGS*)input)->employees;
	t_table = ((THREAD_ARGS*)input)->trips;

	print_thread_info(*(THREAD_ARGS*)input);

	// read the employee table and make a copy of a partition
	e_table = read_employee_table(e_table, start, len);

	sort_employee(e_table, len, id);

	return NULL;
}


/*************************************************************************************
 * Sort the employee partition
 */
void sort_employee(EMPLOYEE * part, int len, int id) {

	int i;

	bool sorted = false;

#if DEBUG
	printf("Presort:\n");
	for(i = 0; i < len; i++) {
		printf("  %d %s\n", part[i].id, part[i].name);
	}
#endif

	// simple swap sort
	while (sorted == false) {
		sorted = true;
		for (i = 1; i < len; i++) {
			if (part[i].id < part[i-1].id) {
				swap_employee(&part[i], &part[i-1]);
				sorted = false;
			}
		}
	}

#if DEBUG
	printf("Endsort:\n");
		for(i = 0; i < len; i++) {
			printf("  %d: %d %s\n", id, part[i].id, part[i].name);
		}
#endif
}


/*************************************************************************************
 * Swap two employee entries
 *
 */
void swap_employee(EMPLOYEE * dest, EMPLOYEE * src) {
	EMPLOYEE temp;

	temp.id = dest->id;	// dest -> temp
	dest->id = src->id; // src -> dest
	src->id = temp.id;	// temp -> src

	strncpy(temp.name, dest->name, SIZE_NAME);
	strncpy(dest->name, src->name, SIZE_NAME);
	strncpy(src->name, temp.name, SIZE_NAME);
}


/*************************************************************************************
 * Copy a partition of the employee table
 * Returns a pointer to the new partition
 */
EMPLOYEE * read_employee_table(EMPLOYEE * table, int start, int len) {

	int i, j, id;
	EMPLOYEE * e_table;

	// allocate memory for table partition
	e_table = (EMPLOYEE*) malloc(sizeof(EMPLOYEE) * len);

	j = 0;
	for (i = start; i < (start + len); i++) {
		e_table[j].id = table[i].id;
		strncpy(e_table[j].name, table[i].name, SIZE_NAME);
		j++;
	}

	return e_table;
}


/**************************************************************************************
 * Copy a partition of the trips table
 * Returns a pointer to the new partition
 */
TRIP * read_trips_table(TRIP * table, int start, int len) {

	int i, j, id;
	TRIP * t_table;

	// allocate memory for table partition
	t_table = (TRIP*) malloc(sizeof(TRIP) * len);

	j = 0;
	for (i = start; i < (start + len); i++) {
		t_table[j].id = table[i].id;
		strncpy(t_table[j].dest, table[i].dest, SIZE_NAME);
		j++;
	}

	return t_table;
}


/**************************************************************************************
 * Print out thread info for debugging
 */
void print_thread_info(THREAD_ARGS args) {
	printf("Thread %d: Start: %d Len: %d\n", args.id, args.start, args.len);
}
