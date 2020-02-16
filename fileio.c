#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "table.h"
#include "datatypes.h"
#include "fileio.h"


#define LEN 100

#define EMPLOYEE_FILE "/media/ryan/Shared/school/588/project/588project/employees.txt"
#define TRIPS_FILE "/media/ryan/Shared/school/588/project/588project/trips.txt"


#define DEBUG 0




/**************************************************************************************
 *
 */
void write_table_to_file(ENTRY * table, char * filename, int tabletype) {

	int i, tsize;
	FILE * file;

	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error writing to file\n");
		return;
	}

	if (tabletype == TABLE_TRIPS) {
		tsize = MAX_TRIPS;
		printf("Writing sorted trips to file\n");
	}
	else {
		tsize = MAX_EMPLOYEES;
		printf("Writing sorted employees to file\n");
	}

	for (i = 0; i < tsize; i++) {
		fprintf(file, "\n%d %s", table[i].id, table[i].desc);
	}


	fclose(file);
	return;
}


/**************************************************************************************
 *
 */
void write_partition_to_file(PARTITION part, int tabletype, int partID) {

	int i, tsize;
	FILE * file;

	if (tabletype == TABLE_EMPLOYEES) {
		switch (partID) {
			case 0:		file = fopen("emp_part0.txt", "w"); break;
			case 1:		file = fopen("emp_part1.txt", "w"); break;
			case 2:		file = fopen("emp_part2.txt", "w"); break;
			case 3:		file = fopen("emp_part3.txt", "w"); break;
			case 4:		file = fopen("emp_part4.txt", "w"); break;
			case 5:		file = fopen("emp_part5.txt", "w"); break;
			case 6:		file = fopen("emp_part6.txt", "w"); break;
			case 7:		file = fopen("emp_part7.txt", "w"); break;
			case 8:		file = fopen("emp_part8.txt", "w"); break;
			case 9:		file = fopen("emp_part9.txt", "w"); break;
			case 10:	file = fopen("emp_part10.txt", "w"); break;
			case 11:	file = fopen("emp_part11.txt", "w"); break;
			case 12:	file = fopen("emp_part12.txt", "w"); break;
			case 13:	file = fopen("emp_part13.txt", "w"); break;
			case 14:	file = fopen("emp_part14.txt", "w"); break;
			case 15:	file = fopen("emp_part15.txt", "w"); break;
		}
	}
	else {
		switch (partID) {
			case 0:		file = fopen("trip_part0.txt", "w"); break;
			case 1:		file = fopen("trip_part1.txt", "w"); break;
			case 2:		file = fopen("trip_part2.txt", "w"); break;
			case 3:		file = fopen("trip_part3.txt", "w"); break;
			case 4:		file = fopen("trip_part4.txt", "w"); break;
			case 5:		file = fopen("trip_part5.txt", "w"); break;
			case 6:		file = fopen("trip_part6.txt", "w"); break;
			case 7:		file = fopen("trip_part7.txt", "w"); break;
			case 8:		file = fopen("trip_part8.txt", "w"); break;
			case 9:		file = fopen("trip_part9.txt", "w"); break;
			case 10:	file = fopen("trip_part10.txt", "w"); break;
			case 11:	file = fopen("trip_part11.txt", "w"); break;
			case 12:	file = fopen("trip_part12.txt", "w"); break;
			case 13:	file = fopen("trip_part13.txt", "w"); break;
			case 14:	file = fopen("trip_part14.txt", "w"); break;
			case 15:	file = fopen("trip_part15.txt", "w"); break;
		}
	}

	if (file == NULL) {
		printf("Error writing to file\n");
		return;
	}

	fprintf(file, "%s %d\n", "Partition", partID);

	for (i = 0; i < part.len; i++) {
		fprintf(file, "\n%d %s", part.entry[i].id, part.entry[i].desc);
	}


	fclose(file);
	return;
}


/**************************************************************************************
 *
 */
int file_load_employees_table(ENTRY ** table){

	int index = 0;
	int id;
	char str [LEN];
	char read [LEN];
	FILE * file;

	file = fopen(EMPLOYEE_FILE, "r");
	if (file == NULL) {
		perror ("Error occurred");
		return -1;
	}

	// allocate memory
	*table = (ENTRY*) malloc(sizeof(ENTRY) * MAX_EMPLOYEES);

	// read the table from the file
	index = 0;
	while (!feof(file)) {

		// read a line
		fgets(read, LEN, file);
		// get the items
		id = atoi(strtok(read, " "));
		strncpy(str, strtok(NULL, "\n"), SIZE_NAME);

		// add the table entry
		((*table) + index)->id = id;
		strncpy(((*table) + index)->desc, str, SIZE_NAME);
		#if DEBUG
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->name);
		#endif
		index++;
	} 

	fclose(file);
	printf("Finished loading employee table\n");
	return 0;
}


/**************************************************************************************
 *
 */
int file_load_trips_table(ENTRY ** table){

	int index = 0;
	int id = 0;
	char * token;
	char * ptr;
	char str [LEN];
	char read [LEN];
	char delimiters[] = " \0";
	FILE * file;

	file = fopen(TRIPS_FILE, "r");
	if (file == NULL) {
		perror ("Error occurred");
		return -1;
	}

	// allocate memory
	*table = (ENTRY*) malloc(sizeof(ENTRY) * MAX_TRIPS);

	// read the table from the file
	index = 0;
	while (!feof(file)) {

		// read a line
		fgets(read, LEN, file);
		// get the items
		id = atoi(strtok(read, " "));
		strncpy(str, strtok(NULL, "\n"), SIZE_CITY);

		// add the table entry
		((*table) + index)->id = id;
		strncpy(((*table) + index)->desc, str, SIZE_CITY);
		#if DEBUG
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->dest);
		#endif
		index++;
	} 

	fclose(file);
	printf("Finished loading trips table\n");
	return 0;
}

