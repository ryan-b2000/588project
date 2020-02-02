#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define SIZE_CITY 30
#define SIZE_NAME 20
#define MAX_EMPLOYEES 20000
#define MAX_TRIPS 100000
#define MAX_DESTINATIONS 386
#define MAX_NAMES 5000

/**   THIS FILE GENERATES THE RANDOM TABLES 
 * 	  Use an argument to the executable for the random seed.
 * 		
 */



//======================================================================//

int seed = 0;
char city_table [MAX_DESTINATIONS][SIZE_CITY];
char name_table [MAX_NAMES][SIZE_NAME];


//======================================================================//
void set_seed(int s);
int get_random_id();
int get_random_destination(char * dest);
int get_random_name(char * dest);


//======================================================================//
/** 
 * @param	argv[1] = random seed
 * @param
 * @return
 */
int main (int argc, char *argv[]) {

	int seed;
	int id;
	int i;
	char name [SIZE_NAME];
	char dest [SIZE_CITY];
	FILE * file;

	// set the random seed
	if (argc > 1)
		set_seed(atoi(argv[1]));
	else
		set_seed(1);

	// Make the Trips table file
	//-----------------------------------------------------------------
	// open trips table file
	file = fopen("trips.txt", "w");
	if (file == NULL) {
		printf("Error opening file.");
		return -1;
	}

	// write random entries to the file
	for (i = 0; i < MAX_TRIPS; i++) {
		id = get_random_id();
		get_random_destination(dest);
		printf("%d %s", id, dest);
		if (fprintf(file, "%d %s", id, dest) == -1) {
			printf("Error writing to file.\n");
			return -1;
		}
	}

	fclose(file);

	// Make the Employees table file
	//-----------------------------------------------------------------
	// open employees table file
	file = fopen("employees.txt", "w");
	if (file == NULL) {
		printf("Error opening file.");
		return -1;
	}

	// write random entries to the file
	for (i = 0; i < MAX_EMPLOYEES; i++) {
		id = get_random_id();
		get_random_name(name);
		printf("%d %s", id, name);
		if (fprintf(file, "%d %s", id, name) == -1) {
			printf("Error writing to file.\n");
			return -1;
		}
	}

	fclose(file);

	return 0;
}


// Constrain the random employee number to be between 1 - MAX
int get_random_id() {
	
	long int rand = 0;

	while (rand < 1 || rand > MAX_EMPLOYEES)
		rand = random();

	return (int)rand;
}


// return a random destination from the list
int get_random_destination(char * dest) {

	static bool table_loaded = false;
	FILE * file;
	char buff [SIZE_CITY];

	if (table_loaded == false) {
		// open the file
		file = fopen("cities.txt", "r");
		// load the city data
		int i;
		
		for (i = 0; i < MAX_DESTINATIONS; i++ ){
			fgets(buff, SIZE_CITY, file);
			/*
			int len = strlen(buff);
			if (len > 0 && buff[len-1] == '\n')
				buff[--len] = '\0';
			*/
			strncpy(city_table[i], buff, SIZE_CITY);
		}
		table_loaded = true;
		printf("City table loaded\n");
		fclose(file);
	}

	// get a random city name
	long int rand = 0;

	do {
		rand = random();
	} while (rand > MAX_DESTINATIONS);

	strncpy(dest, city_table[rand], 30);

	return 0;
}


// return a random destination from the list
int get_random_name(char * dest) {

	int i;
	char str [SIZE_NAME];
	static bool table_loaded = false;
	FILE * file;

	if (table_loaded == false) {
		file = fopen("names.txt", "r");

		for (i = 0; i < MAX_NAMES; i++ ){
			fgets(str, SIZE_NAME, file);
			strncpy(name_table[i], str, SIZE_NAME);
		}
		table_loaded = true;
		printf("Name table loaded\n");
		fclose(file);
	}

	// get a random city name
	long int rand = 0;

	do {
		rand = random();
	} while (rand > MAX_NAMES);

	strncpy(dest, name_table[rand], 30);

	return 0;
}

void set_seed(int s) {
	seed = s;
	srandom(seed);
}
