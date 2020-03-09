/*		Ryan Bentz, Ignacio Roberto Genovese, Rezwana Mahbub
 * 		ECE 588 Final Project
 * 		2020/03/05
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "table.h"
#include "datatypes.h"
#include "fileio.h"


//======================================================================//
#define LEN 100
#define PATH_SIZE 1000

#define EMPLOYEE_FILE "employees.txt"
#define TRIPS_FILE "trips.txt"

#define DEBUG 0


//======================================================================//
extern int numprocs;
extern MERGED_ENTRIES * merged;



/**************************************************************************************
 *
 */
void write_merged_table_to_file(char * filename) {

	int key, c;
	int count, len;
	char * str;
	FILE * file;

	file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error writing to file\n");
		return;
	}

	for (key = 0; key < MAX_EMPLOYEES; key++) {

		// write the names list from the merged entries
		count = merged[key].employees.count;
		if (count == 0)
			fprintf(file, "%6d NAMES:  NONE", key);
		else {
			fprintf(file, "%6d NAMES: ", key);
			for (c = 0; c < count; c++)
				fprintf(file, " %s,", merged[key].employees.desc[c]);
		}
		fprintf(file, "\n");

		// write the cities list from the merged entries
		count = merged[key].trips.count;
		if (count == 0)
			fprintf(file, "       CITIES: NONE");
		else {
			fprintf(file, "       CITIES:");
			for (c = 0; c < count; c++)
				fprintf(file, " %s,", merged[key].trips.desc[c]);
		}
		fprintf(file, "\n");


	}


	fclose(file);
	return;
}


/**************************************************************************************
 *
 */
void write_employee(FILE * file, int key, MERGED_ENTRIES * merged) {

	int count = merged[key].employees.count;
	int i = 0;

	fprintf(file, "%d: %s", key, merged[key].employees.desc[0]);

}


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
void write_partition_to_file(ENTRY * table, int tabletype, int partID) {

	int i, len, start;
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

	if (tabletype == TABLE_EMPLOYEES) {
		len = MAX_EMPLOYEES / numprocs;
	}
	else {
		len = MAX_TRIPS / numprocs;
	}
	start = partID * len;

	fprintf(file, "%d %s", table[start].id, table[start].desc);
	for (i = start + 1; i < (start + len); i++) {
		fprintf(file, "\n%d %s", table[i].id, table[i].desc);
	}


	fclose(file);
	return;
}


/**************************************************************************************
 *
 */
int file_load_input_table(ENTRY ** table, int type){

	int index = 0;
	int id = 0;
	char * token;
	char * ptr;
	char str [LEN];
	char read [LEN];
	char delimiters[] = " \0";
	char * filename;
	char path [PATH_SIZE];
	int pathlen, filelen;
	FILE * file;


	// get the current path
	if (getcwd(path, PATH_SIZE) < 0) {
		printf("Error getting current working directory.\n");
		return -1;
	}

	pathlen = strlen(path);

	if (type == TABLE_TRIPS) {
		// create the absolute path filename
		filelen = strlen(TRIPS_FILE);
		filename = (char*) malloc (sizeof(char) * (pathlen + filelen + 1));

		strncpy(filename, path, strlen(path));
		strncat(filename, "/", 1);
		strncat(filename, TRIPS_FILE, filelen);

		// allocate memory
		*table = (ENTRY*) malloc(sizeof(ENTRY) * MAX_TRIPS);
	}
	else {
		// create the absolute path filename
		filelen = strlen(EMPLOYEE_FILE);
		filename = (char*) malloc (sizeof(char) * (pathlen + filelen + 1));

		strncpy(filename, path, strlen(path));
		strncat(filename, "/", 1);
		strncat(filename, EMPLOYEE_FILE, filelen);

		// allocate memory
		*table = (ENTRY*) malloc(sizeof(ENTRY) * MAX_EMPLOYEES);
	}

	printf("Open file: %s\n", filename);
	file = fopen(filename, "r");
	if (file == NULL) {
		perror ("Error occurred");
		return -1;
	}

	// read the table from the file
	index = 0;
	while (!feof(file)) {

		// read a line
		fgets(read, LEN, file);
		// get the items
		id = atoi(strtok(read, " "));
		strncpy(str, strtok(NULL, "\n"), SIZE_DESC);

		// add the table entry
		((*table) + index)->id = id;
		strncpy(((*table) + index)->desc, str, SIZE_DESC);
		#if 0
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->desc);
		#endif
		index++;
	}

	memset(filename, '\0', strlen(filename));
	free(filename);

	fclose(file);
	printf("Finished loading input table\n");
	return 0;
}



/**************************************************************************************
 *
 */
int file_load_employees_table(ENTRY ** table){

	int index = 0;
	int id, i;
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
	//while (!feof(file)) {
	for (i = 0; i < MAX_EMPLOYEES; i++) {
		// read a line
		fgets(read, LEN, file);
		// get the items
		id = atoi(strtok(read, " "));
		strncpy(str, strtok(NULL, "\n"), SIZE_NAME);

		// add the table entry
		((*table) + index)->id = id;
		strncpy(((*table) + index)->desc, str, SIZE_NAME);
		#if 0
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->desc);
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
		#if 0
			printf("Read %d: %d %s\n", index+1, ((*table) + index)->id, ((*table) + index)->desc);
		#endif
		index++;
	} 

	fclose(file);
	printf("Finished loading trips table\n");
	return 0;
}

