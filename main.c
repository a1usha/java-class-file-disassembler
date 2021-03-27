/**
 * A program to output the contents of constant_pool 
 * by parsing java .class files as that's made in javap tool. 
 * 
 * This code is written as an assignment to understand 
 * the structure of the class files.
 * 
 * Author: Aleksandr Ushaev
 * Created: 14.02.2020
 * 
 */

#include "class_reader.h"
#include "pretty_printer.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
    }
    else
    {
        FILE *class_file = open_class_file(argv[1]);
        class *cls = parse_class_file(class_file);
        print_version_info(stdout, cls);
        print_constant_pool(stdout, cls);
    }

    return 0;
}