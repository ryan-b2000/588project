/*		Ryan Bentz, Ignacio Roberto Genovese, Rezwana Mahbub
 * 		ECE 588 Final Project
 * 		2020/03/05
 */

#ifndef __DATATYPES__
#define __DATATYPES__


//======================================================================//
#define MAX_EMPLOYEES 20000
#define MAX_TRIPS 100000
#define MAX_DESTINATIONS 386
#define MAX_NAMES 5000

#define MAX_ENTRIES MAX_EMPLOYEES

#define SIZE_CITY 30
#define SIZE_NAME 20
#define SIZE_DESC SIZE_CITY

#define TABLE_TRIPS 			0
#define TABLE_EMPLOYEES 	1

//======================================================================//

typedef struct entry_t {
	int id;
	char desc [SIZE_DESC];
} ENTRY;

typedef struct thread_args_t {
	int id;										// the thread ID
} THREAD_ARGS;

typedef struct subpart_t {
	ENTRY * start_ptr;			// starting index of sub-partition
	int start;							// starting address of the sub-partition (relative to partition)
	int len;								// length of sub-partition
} SUBPART;

typedef struct part_t {
	//ENTRY * start_ptr;
	int start;							// starting index of the partition (relative to the entire table)
	int len;								// master size of partition
	SUBPART * subpart;			// array for sub-partition information
} PARTITION;


typedef struct m_entry_t {
	int count;							// number of entries of this type
	char ** desc;						// array for each entry
} M_ENTRY;

typedef struct merged_t {
	M_ENTRY trips;
	M_ENTRY employees;
} MERGED_ENTRIES;


typedef struct histogram_t {
	int count;	// number of keys the fall into this bucket
	int high;		// high value of the range (inclusive)
	int low;		// low value of the range (inclusive)
} HISTOGRAM;

#endif // DATATYPES
