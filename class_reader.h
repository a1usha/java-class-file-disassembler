#ifndef CLASS_READER_H
#define CLASS_READER_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <endian.h>

#include <string.h>

#define MAGIC_NUMBER 0xcafebabe

typedef struct CONSTANT_Ref_info_s
{
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;

} CONSTANT_Ref_info;

typedef struct CONSTANT_Class_info_s
{
    uint8_t tag;
    uint16_t name_index;

} CONSTANT_Class_info;

typedef struct CONSTANT_String_info_s
{
    uint8_t tag;
    uint16_t string_index;

} CONSTANT_String_info;

typedef struct CONSTANT_IntFloat_info_s
{
    uint8_t tag;
    uint32_t bytes;

} CONSTANT_IntFloat_info;

typedef struct CONSTANT_LongDouble_info_s
{
    uint8_t tag;
    uint32_t high_bytes;
    uint32_t low_bytes;

} CONSTANT_LongDouble_info;

typedef struct CONSTANT_Utf8_info_s
{
    uint8_t tag;
    uint16_t length;
    char *bytes;

} CONSTANT_Utf8_info;

typedef struct CONSTANT_MethodHandle_info_s
{
    uint8_t tag;
    uint8_t reference_kind;
    uint16_t reference_index;

} CONSTANT_MethodHandle_info;

typedef struct CONSTANT_MethodType_info_s
{
    uint8_t tag;
    uint16_t descriptor_index;

} CONSTANT_MethodType_info;

typedef struct CONSTANT_InvokeDynamic_info_s
{
    uint8_t tag;
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
    
} CONSTANT_InvokeDynamic_info;

typedef union constant_info_u
{
    CONSTANT_Ref_info ref_i;
    CONSTANT_Class_info class_i;
    CONSTANT_String_info string_i;
    CONSTANT_IntFloat_info int_float_i;
    CONSTANT_LongDouble_info long_double_i;
    CONSTANT_Utf8_info utf_i;
    CONSTANT_MethodHandle_info method_handle_i;
    CONSTANT_MethodType_info method_type_i;
    CONSTANT_InvokeDynamic_info invoke_dynamic_i;

} constant_info;

typedef enum constant_pool_tag_e
{
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18

} constant_pool_tag;

static char *reference_kind[] = 
{
    "REF_getField", // 1
    "REF_getStatic", // 2
    "REF_putField", // 3
    "REF_putStatic", // 4
    "REF_invokeVirtual", // 5
    "REF_invokeStatic", // 6
    "REF_invokeSpecial", // 7
    "REF_newInvokeSpecial", // 8
    "REF_invokeInterface" // 9
};

typedef enum inf_nan_e
{
    NORMAL = 0,
    POSITIVE_INFINITY = 1,
    NEGATIVE_INFINITY = 2,
    NAN = 3

} inf_nan;

typedef struct class_s
{
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    constant_info *constant_pool;

} class;

typedef union double_cast_u
{
    double d;
    struct parts_s
    {
        uint64_t mantisa : 52;
        uint32_t exponent : 11;
        uint32_t sign : 1;

    } parts;
    
} double_cast;

FILE *open_class_file(char *path);
bool is_class_file(FILE *file);
class *parse_class_file(FILE *class_file);
void parse_u2(FILE *class_file, uint16_t *cls);
void parse_constant_pool(FILE *class_file, class *cls);
void print_constant_pool(FILE *stream, class *cls);

#endif