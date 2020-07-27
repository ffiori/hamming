#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

size_t get_file_size(FILE *fp);
char *array_from_file(const char *filename, int *size);

char *byte_to_bit_from_file(const char *filename, int *len);
char *byte_to_bit_from_string(const char *str, int *len);

long getrss();

int nearest_power_of_two(int v);

#ifdef __cplusplus
}
#endif

#endif
