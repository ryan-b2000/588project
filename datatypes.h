#ifndef __DATATYPES__
#define __DATATYPES__


//======================================================================//
#define MAX_EMPLOYEES 20000
#define MAX_TRIPS 100000
#define MAX_DESTINATIONS 386
#define MAX_NAMES 5000

#define SIZE_CITY 30
#define SIZE_NAME 20
#define SIZE_DESC SIZE_CITY

//======================================================================//
typedef struct employee_t {
	int id;
	char name [SIZE_NAME];
} EMPLOYEE;


typedef struct trip_t {
	int id;
	char dest [SIZE_CITY];
} TRIP;

typedef struct entry_t {
	int id;
	char desc [SIZE_DESC];
} ENTRY;


#endif // DATATYPES
