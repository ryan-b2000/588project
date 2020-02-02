#ifndef __DATATYPES__
#define __DATATYPES__


//======================================================================//
#define MAX_EMPLOYEES 20000
#define MAX_TRIPS 100000
#define MAX_DESTINATIONS 386
#define MAX_NAMES 5000

#define SIZE_CITY 30
#define SIZE_NAME 20


//======================================================================//
typedef struct employee_t {
	int id;
	char name [20]; 
} EMPLOYEE;


typedef struct trip_t {
	int id;
	char dest [30];
} TRIP;



#endif // DATATYPES
