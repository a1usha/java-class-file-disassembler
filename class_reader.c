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

/**
 * Attempts to open a file, on success checks 
 * if the file is a valid .class file.
 * 
 * @param path of the file to open
 * @return file if it is valid .class file
 */
FILE *open_class_file(char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        perror("Error ");
        exit(EXIT_FAILURE);
    }

    if (!is_class_file(file))
    {
        fprintf(stderr, "This file is not a .class file!\n");
        exit(0);
    }

    return file;
}

/**
 * Checks the file for a magic value 0xcafebabe
 * 
 * @param file to check
 * @return true if file contatins magic value
 */
bool is_class_file(FILE *file)
{
    uint32_t magic_number;
    size_t read_cnt = fread(&magic_number, sizeof(uint32_t), 1, file);
    magic_number = be32toh(magic_number);
    return read_cnt == 1 && magic_number == MAGIC_NUMBER;
}

/**
 * Reads minor, major versions, constant pool size
 * and constant pool from a file
 * 
 * @param file to read from
 * @return class struct filled with collected data
 */
class *parse_class_file(FILE *class_file)
{
    class *cls = (class *)malloc(sizeof(class));

    // Read header
    parse_u2(class_file, &cls->minor_version);
    parse_u2(class_file, &cls->major_version);
    parse_u2(class_file, &cls->constant_pool_count);

    parse_constant_pool(class_file, cls);
    fclose(class_file);
    return cls;
}

/**
 * Reads an "unsigned two-byte quantity" from a file
 * 
 * @param file to read from
 * @param variable to write
 */
void parse_u2(FILE *class_file, uint16_t *cls)
{
    fread(cls, sizeof(uint16_t), 1, class_file);
    (*cls) = be16toh((*cls));
}

/**
 * Parse symbolic information from the "constant_pool" table
 * from a file
 * 
 * @param file to read from
 * @param class struct to write
 */
void parse_constant_pool(FILE *class_file, class *cls)
{
    const uint16_t total_constants_count = cls->constant_pool_count - 1;
    cls->constant_pool = malloc(total_constants_count * sizeof(constant_info));
    uint8_t tag;

    CONSTANT_Class_info class_info;
    CONSTANT_Ref_info ref_info;
    CONSTANT_String_info string_info;
    CONSTANT_IntFloat_info int_float_info;
    CONSTANT_LongDouble_info long_double_info;
    CONSTANT_Utf8_info utf_info;
    CONSTANT_MethodHandle_info method_handle_info;
    CONSTANT_MethodType_info method_type_info;
    CONSTANT_InvokeDynamic_info invoke_info;

    for (int i = 1; i <= total_constants_count; i++)
    {
        constant_info *cur_constant_info = cls->constant_pool + (i - 1);
        fread(&tag, sizeof(uint8_t), 1, class_file);

        switch (tag)
        {
        case CONSTANT_Class:
            fread(&class_info.name_index, sizeof(class_info.name_index), 1, class_file);
            class_info.tag = tag;
            class_info.name_index = be16toh(class_info.name_index);
            cur_constant_info->class_i = class_info;
            break;

        case CONSTANT_Fieldref:
        case CONSTANT_InterfaceMethodref:
        case CONSTANT_Methodref:
        case CONSTANT_NameAndType:
            fread(&ref_info.class_index, sizeof(ref_info.class_index), 1, class_file);
            fread(&ref_info.name_and_type_index, sizeof(ref_info.name_and_type_index), 1, class_file);
            ref_info.tag = tag;
            ref_info.class_index = be16toh(ref_info.class_index);
            ref_info.name_and_type_index = be16toh(ref_info.name_and_type_index);
            cur_constant_info->ref_i = ref_info;
            break;

        case CONSTANT_String:
            fread(&string_info.string_index, sizeof(string_info.string_index), 1, class_file);
            string_info.tag = tag;
            string_info.string_index = be16toh(string_info.string_index);
            cur_constant_info->string_i = string_info;
            break;

        case CONSTANT_Integer:
        case CONSTANT_Float:
            fread(&int_float_info.bytes, sizeof(int_float_info.bytes), 1, class_file);
            int_float_info.tag = tag;
            int_float_info.bytes = be32toh(int_float_info.bytes);
            cur_constant_info->int_float_i = int_float_info;
            break;

        case CONSTANT_Long:
        case CONSTANT_Double:
            fread(&long_double_info.high_bytes, sizeof(long_double_info.high_bytes), 1, class_file);
            fread(&long_double_info.low_bytes, sizeof(long_double_info.low_bytes), 1, class_file);
            long_double_info.tag = tag;
            long_double_info.high_bytes = be32toh(long_double_info.high_bytes);
            long_double_info.low_bytes = be32toh(long_double_info.low_bytes);
            cur_constant_info->long_double_i = long_double_info;
            i++; // Takes two entries
            break;

        case CONSTANT_Utf8:
            fread(&utf_info.length, sizeof(utf_info.length), 1, class_file);
            utf_info.length = be16toh(utf_info.length);
            utf_info.bytes = malloc(sizeof(char) * (utf_info.length + 1));
            fread(utf_info.bytes, sizeof(char), utf_info.length, class_file);
            utf_info.bytes[utf_info.length] = '\0';
            utf_info.tag = tag;
            cur_constant_info->utf_i = utf_info;
            break;

        case CONSTANT_MethodHandle:
            fread(&method_handle_info.reference_kind, sizeof(method_handle_info.reference_kind), 1, class_file);
            fread(&method_handle_info.reference_index, sizeof(method_handle_info.reference_index), 1, class_file);
            method_handle_info.reference_index = be16toh(method_handle_info.reference_index);
            method_handle_info.tag = tag;
            cur_constant_info->method_handle_i = method_handle_info;
            break;

        case CONSTANT_MethodType:
            fread(&method_type_info.descriptor_index, sizeof(method_type_info.descriptor_index), 1, class_file);
            method_type_info.descriptor_index = be16toh(method_type_info.descriptor_index);
            method_type_info.tag = tag;
            cur_constant_info->method_type_i = method_type_info;
            break;

        case CONSTANT_InvokeDynamic:
            fread(&invoke_info.bootstrap_method_attr_index, sizeof(invoke_info.bootstrap_method_attr_index), 1, class_file);
            fread(&invoke_info.name_and_type_index, sizeof(invoke_info.name_and_type_index), 1, class_file);
            invoke_info.bootstrap_method_attr_index = be16toh(invoke_info.bootstrap_method_attr_index);
            invoke_info.name_and_type_index = be16toh(invoke_info.name_and_type_index);
            invoke_info.tag = tag;
            cur_constant_info->invoke_dynamic_i = invoke_info;
            break;

        default:
            fprintf(stderr, "Don't know what to do with %d tag byte :(\n", tag);
            cur_constant_info = NULL;
            break;
        }
    }
}