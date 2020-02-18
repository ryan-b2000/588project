#include "datatypes.h"


#define TABLE_TRIPS 			0
#define TABLE_EMPLOYEES 	1

/**************************************************************************************
 *
 */

void init_subparts(int id, ENTRY * table, int tabletype);

void verify_subparts(int id, ENTRY * table, int tabletype);

void split_and_sort_tables();

void merge_sorted_partitions (ENTRY * table, int tabletype);

void merge_entries(int key, int id, MERGED_ENTRIES * entry, int type);

void assign_key_ranges(int * range);

void merge_join(int id, ENTRY * table, int tabletype);

void merge_employee_entries(int key, int id);

void merge_trip_entries(int key, int id);

void merge_join_tables(ENTRY ** etable, ENTRY ** ttable);

void print_table(ENTRY * partition, int tabletype);

void print_partition(PARTITION partition, int id);

int get_next_key(PARTITION partition);

int count_key_matches(int id, int key);

int add_sorted_entries(PARTITION * partition, ENTRY * table, int * index);
