#ifndef __FILE_IO__
#define __FILE_IO__

#include "datatypes.h"



int file_load_employees_table(ENTRY ** table);

int file_load_trips_table(ENTRY ** table);

void write_table_to_file(ENTRY * table, char * filename, int tabletype);

void write_partition_to_file(ENTRY * table, int tabletype, int partID);

void write_merged_table_to_file(char * filename);

#endif //__FILE_IO__
