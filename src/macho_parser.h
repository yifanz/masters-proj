#ifndef macho_parser_h
#define macho_parser_h

#include <stdio.h>
#include <stdlib.h>
#include <mach-o/loader.h>
#include <mach-o/swap.h>
#include <mach-o/fat.h>

void dump_segments(FILE *obj_file);
int read_entry_point_cmd(FILE *obj_file, struct entry_point_command* entry_point);

#endif /* macho_parser_h */
