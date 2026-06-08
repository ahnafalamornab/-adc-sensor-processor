#ifndef IO_H
#define IO_H

#include "adc.h"

ADCSample *load_binary(const char *filename, int *count);
void export_results(const char *filename, ADCSample *samples, int count);

#endif