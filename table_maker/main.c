#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define SIZE_DESC 30
#define SIZE_CITY 30
#define SIZE_NAME 20
#define MAX_EMPLOYEES 20000
#define MAX_TRIPS 100000
#define MAX_DESTINATIONS 380
#define MAX_NAMES 5000

/**   THIS FILE GENERATES THE RANDOM TABLES
 * 	  Use an argument to the executable for the random seed.
 *
 */



//======================================================================//

int seed = 0;
char city_table [MAX_DESTINATIONS][SIZE_CITY];
char name_table [MAX_NAMES][SIZE_NAME];


//==================z====================================================//
void set_seed(int s);
int get_random_id();
int get_random_destination(char * dest);
int get_random_name(char * dest);

void make_trips();
void make_employees();

//======================================================================//
/**
 * @param	argv[1] = random seed
 * @param
 * @return
 */
int main (int argc, char *argv[]) {

	int seed;

	// set the random seed
	if (argc > 1)
		set_seed(atoi(argv[1]));
	else
		set_seed(1);

	make_trips();

	//make_employees();

	return 0;
}

void make_trips() {

	int i, id;
	FILE * tripsfile;
	FILE * cityfile;
	char buff [SIZE_DESC];
	char desc [SIZE_DESC];

	// Make the Trips table file
	//-----------------------------------------------------------------
	// open trips table file
	tripsfile = fopen("trips.txt", "w");
	if (tripsfile == NULL) {
		printf("Error opening file.");
		return;
	}

	// open the city file
	cityfile = fopen("cities.txt", "r");


	for (i = 0; i < MAX_DESTINATIONS; i++ ){
		fgets(buff, SIZE_CITY, cityfile);
		/*
		int len = strlen(buff);
		if (len > 0 && buff[len-1] == '\n')
			buff[--len] = '\0';
		*/
		strncpy(city_table[i], buff, SIZE_CITY);
	}
	printf("City table loaded\n");
	fclose(cityfile);




	// write random entries to the file
	for (i = 0; i < MAX_TRIPS; i++) {
		id = get_random_id();
		get_random_destination(desc);
		//printf("%d %s", id, desc);
		if (fprintf(tripsfile, "%d %s", id, desc) == -1) {
			printf("Error writing to file.\n");
			return;
		}
	}

	fclose(tripsfile);
}

void make_employees() {

	int id;
	int i;
	char name [SIZE_NAME];
	char buff [SIZE_CITY];

	FILE * employeesfile;
	FILE * namefile;

	// Make the Employees table file
	//-----------------------------------------------------------------
	// open employees table file
	employeesfile = fopen("employees.txt", "w");
	if (employeesfile == NULL) {
		printf("Error opening file.");
		return;
	}

	namefile = fopen("names.txt", "r");
	if (namefile == NULL) {
			printf("Error opening file.");
			return;
		}

	for (i = 0; i < MAX_NAMES; i++ ){
		fgets(buff, SIZE_DESC, namefile);
		strncpy(name_table[i], buff, SIZE_DESC);
	}

	printf("Name table loaded\n");
	fclose(namefile);


	// write random entries to the file
	for (i = 0; i < MAX_EMPLOYEES; i++) {
		id = get_random_id();
		get_random_name(name);
		//printf("%d %s", id, name);
		if (fprintf(employeesfile, "%d %s", id, name) == -1) {
			printf("Error writing to file.\n");
			return;
		}
	}

	fclose(employeesfile);

	return;
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

	// get a random city name
	long int rand = 0;

	do {
		rand = random();
	} while (rand >= MAX_DESTINATIONS);

	strncpy(dest, city_table[rand], 30);

	return 0;
}


// return a random destination from the list
int get_random_name(char * dest) {

	// get a random city name
	long int rand = 0;

	do {
		rand = random();
	} while (rand >= MAX_NAMES);

	strncpy(dest, name_table[rand], 30);

	return 0;
}


void set_seed(int s) {
	seed = s;
	srandom(seed);
}
