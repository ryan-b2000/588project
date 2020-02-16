#include "datatypes.h"


#define TABLE_TRIPS 			0
#define TABLE_EMPLOYEES 	1

/**************************************************************************************
 *
 */
void split_and_sort_table(ENTRY * table, int tabletype);

void merge_sorted_partitions (ENTRY * table, int tabletype);

void merge_join_tables(ENTRY ** etable, ENTRY ** ttable);

void print_table(ENTRY * partition, int tabletype);

void print_partition(PARTITION partition, int id);

int get_employee_number(PARTITION partition);

int add_sorted_entry(PARTITION * partition, ENTRY * table, int * index);
