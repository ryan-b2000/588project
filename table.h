/*		Ryan Bentz, Ignacio Roberto Genovese, Rezwana Mahbub
 * 		ECE 588 Final Project
 * 		2020/03/05
 */

#include "datatypes.h"




//======================================================================//

void split_and_sort_tables();


void init_subparts(int id, ENTRY * table, int tabletype);


void verify_subparts(int id, ENTRY * table, int tabletype);


void merge_entries(int key, int id, MERGED_ENTRIES * entry, int type);


void merge_join(int id, ENTRY * table, int tabletype);


void print_table(ENTRY * partition, int tabletype);


int count_key_matches(int id, int key, ENTRY * table);


int add_sorted_entries(PARTITION * partition, ENTRY * table, int * index);
