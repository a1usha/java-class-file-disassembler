#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "class_reader.h"

void print_constant_pool(FILE *stream, class *cls);
void print_version_info(FILE *stream, class *cls);

#endif