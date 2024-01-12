#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <stdlib.h>
#include <stdint.h>

// Function declarations
void *_malloc(size_t size, uint32_t line);
void _free(void *ptr, uint32_t line);
void print_report();

// Redefine the standard allocation functions
#define malloc(size) _malloc(size, __LINE__)
#define free(ptr) _free(ptr, __LINE__)

#endif //
