#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "datatypes.h"
#include "fileio.h"


#define LEN 100

#define EMPLOYEE_FILE "/media/ryan/Shared/school/588/project/588project/employees.txt"
#define TRIPS_FILE "/media/ryan/Shared/school/588/project/588project/trips.txt"


#define DEBUG 0


int file_load_employees_table(EMPLOYEE ** table){

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
	*table = (EMPLOYEE*) malloc(sizeof(EMPLOYEE) * MAX_EMPLOYEES);

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
		strncpy(((*table) + index)->name, str, SIZE_NAME);
		#if DEBUG
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->name);
		#endif
		index++;
	} 

	fclose(file);
	printf("Finished loading employee table\n");
	return 0;
}


int file_load_trips_table(TRIP ** table){

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
	*table = (TRIP*) malloc(sizeof(TRIP) * MAX_TRIPS);

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
		strncpy(((*table) + index)->dest, str, SIZE_CITY);
		#if DEBUG
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->dest);
		#endif
		index++;
	} 

	fclose(file);
	printf("Finished loading trips table\n");
	return 0;
}

