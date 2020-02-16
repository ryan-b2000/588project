#ifndef __FILE_IO__
#define __FILE_IO__

#include "datatypes.h"



int file_load_employees_table(ENTRY ** table);

int file_load_trips_table(ENTRY ** table);

void write_table_to_file(ENTRY * table, char * filename, int tabletype);

void write_partition_to_file(PARTITION part, int tabletype, int partID);

#endif //__FILE_IO__
