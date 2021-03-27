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

#include "pretty_printer.h"

/**
 * Converts .class double representation to C double as it specified
 * oracle specification.
 * 
 * @param bits from .class file
 * @return converted double
 */
double long_bits_to_double(long bits)
{
    int s = ((bits >> 63) == 0) ? 2 : 1;
    int e = (int)((bits >> 52) & 0x7ffL);
    long m = (e == 0)
                 ? (bits & 0xfffffffffffffL) << 1
                 : (bits & 0xfffffffffffffL) | 0x10000000000000L;

    double_cast d_cast; // Create double via struct + union
    d_cast.parts.sign = s;
    d_cast.parts.exponent = e;
    d_cast.parts.mantisa = m;

    return d_cast.d;
}

/**
 * Check .class double representation for positive/negative infinity and NaN
 * 
 * @param bits from .class file
 * @return inf_nan field
 */
inf_nan check_long_bits_for_inf_nan(long bits)
{
    if (bits == 0x7ff0000000000000L)
        return POSITIVE_INFINITY;
    else if (bits == 0xfff0000000000000L)
        return NEGATIVE_INFINITY;
    else if ((bits > 0x7ff0000000000000L && bits < 0x7fffffffffffffffL) || (bits > 0xfff0000000000001L && bits < 0xffffffffffffffffL))
        return NAN;
    else
        return NORMAL;
}

/**
 * Get utf string from constant_pool
 * 
 * @param class struct where to seatch
 * @param id of constant_pool info
 * @return utf8 string
 */
char *get_utf(class *cls, int id)
{
    constant_info *cur_constant_info = cls->constant_pool + id;
    char *utf;

    while (cur_constant_info->class_i.tag != CONSTANT_Utf8)
    {
        cur_constant_info = cls->constant_pool + (cur_constant_info->class_i.name_index - 1);
    }

    cur_constant_info->utf_i.bytes[strcspn(cur_constant_info->utf_i.bytes, "\n")] = 0;
    return cur_constant_info->utf_i.bytes;
}

/**
 * Print .class minor and major versions
 * @param stream to write for
 * @param class struct with specified versions 
 */
void print_version_info(FILE *stream, class *cls)
{
    fprintf(stream, "minor version: %d\n", cls->minor_version);
    fprintf(stream, "major version: %d\n\n", cls->major_version);
}

/**
 * Print constant_pool info as it prints in javap tool
 * 
 * @param stream to write for
 * @param class struct
 */
void print_constant_pool(FILE *stream, class *cls)
{
    const uint16_t total_constants_count = cls->constant_pool_count - 1;

    for (int i = 0; i < total_constants_count; i++)
    {
        constant_info *cur_constant_info = cls->constant_pool + i;
        switch (cur_constant_info->class_i.tag)
        {
        case CONSTANT_Class: // done
            fprintf(stream, "#%d = Class\t\t#%d\t\t// %s\n", i + 1,
                    cur_constant_info->class_i.name_index, get_utf(cls, i));
            break;

        case CONSTANT_Fieldref: // done 1-st, done 2-nd
            fprintf(stream, "#%d = Fieldref\t\t#%d.#%d\t\t// %s.%s:%s\n", i + 1,
                    cur_constant_info->ref_i.class_index, cur_constant_info->ref_i.name_and_type_index,
                    get_utf(cls, cur_constant_info->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.name_and_type_index - 1));
            break;

        case CONSTANT_InterfaceMethodref: // done 1-st, done 2-nd
            fprintf(stream, "#%d = InterfaceMethodRef\t#%d.#%d\t\t// %s.%s:%s\n", i + 1,
                    cur_constant_info->ref_i.class_index, cur_constant_info->ref_i.name_and_type_index,
                    get_utf(cls, cur_constant_info->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.name_and_type_index - 1));
            break;

        case CONSTANT_Methodref: // done 1-st, done 2-nd
            fprintf(stream, "#%d = MethodRef\t\t#%d.#%d\t\t// %s.%s:%s\n", i + 1,
                    cur_constant_info->ref_i.class_index, cur_constant_info->ref_i.name_and_type_index,
                    get_utf(cls, cur_constant_info->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->ref_i.name_and_type_index - 1))->ref_i.name_and_type_index - 1));
            break;

        case CONSTANT_NameAndType: // done 1-st, done 2-nd
            fprintf(stream, "#%d = NameAndType\t#%d:#%d\t\t// %s:%s\n", i + 1,
                    cur_constant_info->ref_i.class_index, cur_constant_info->ref_i.name_and_type_index,
                    get_utf(cls, cur_constant_info->ref_i.class_index - 1),
                    get_utf(cls, cur_constant_info->ref_i.name_and_type_index - 1));
            break;

        case CONSTANT_String: // done
            fprintf(stream, "#%d = String\t\t#%d\t\t// %s\n", i + 1,
                    cur_constant_info->string_i.string_index, get_utf(cls, i));
            break;

        case CONSTANT_Integer: // done
            fprintf(stream, "#%d = Integer\t\t%d\n", i + 1,
                    cur_constant_info->int_float_i.bytes);
            break;

        case CONSTANT_Float: // done
            fprintf(stream, "#%d = Float\t\t%f\n", i + 1,
                    (float)cur_constant_info->int_float_i.bytes);
            break;

        case CONSTANT_Long: // done
            fprintf(stream, "#%d = Long\t\t%ldl\n", i + 1,
                    ((long)cur_constant_info->long_double_i.high_bytes << 32) + cur_constant_info->long_double_i.low_bytes);
            i++;
            break;

        case CONSTANT_Double: // done
            switch (check_long_bits_for_inf_nan(((long)cur_constant_info->long_double_i.high_bytes << 32) + cur_constant_info->long_double_i.low_bytes))
            {
            case POSITIVE_INFINITY:
                fprintf(stream, "#%d = Double\t\tInfinityd\n", i + 1);
                break;
            case NEGATIVE_INFINITY:
                fprintf(stream, "#%d = Double\t\t-Infinityd\n", i + 1);
                break;
            case NAN:
                fprintf(stream, "#%d = Double\t\tNaNd\n", i + 1);
                break;
            case NORMAL:
                fprintf(stream, "#%d = Double\t\t%lfd\n", i + 1,
                        long_bits_to_double(((long)cur_constant_info->long_double_i.high_bytes << 32) + cur_constant_info->long_double_i.low_bytes));
                break;
            }
            i++;
            break;

        case CONSTANT_Utf8: // done
            fprintf(stream, "#%d = Utf8\t\t%s\n", i + 1, cur_constant_info->utf_i.bytes);
            break;

        case CONSTANT_MethodHandle: // done
            fprintf(stream, "#%d = MethodHandle\t%d:#%d\t\t// %s %s.%s:%s\n", i + 1,
                    cur_constant_info->method_handle_i.reference_kind, cur_constant_info->method_handle_i.reference_index,
                    reference_kind[cur_constant_info->method_handle_i.reference_kind - 1],
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->method_handle_i.reference_index - 1))->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cls->constant_pool + (cur_constant_info->method_handle_i.reference_index - 1))->ref_i.name_and_type_index - 1)->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cls->constant_pool + (cur_constant_info->method_handle_i.reference_index - 1))->ref_i.name_and_type_index - 1)->ref_i.name_and_type_index - 1));
            break;

        case CONSTANT_MethodType: // done
            fprintf(stream, "#%d = MethodType\t\t%d\t\t// %s\n", i + 1,
                    cur_constant_info->method_type_i.descriptor_index,
                    get_utf(cls, cur_constant_info->method_type_i.descriptor_index - 1));
            break;

        case CONSTANT_InvokeDynamic: // done
            fprintf(stream, "#%d = InvokeDynamic\t#%d:#%d\t\t// #%d:%s:%s\n", i + 1,
                    cur_constant_info->invoke_dynamic_i.bootstrap_method_attr_index,
                    cur_constant_info->invoke_dynamic_i.name_and_type_index,
                    cur_constant_info->invoke_dynamic_i.bootstrap_method_attr_index,
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->invoke_dynamic_i.name_and_type_index - 1))->ref_i.class_index - 1),
                    get_utf(cls, (cls->constant_pool + (cur_constant_info->invoke_dynamic_i.name_and_type_index - 1))->ref_i.name_and_type_index - 1));
            break;

        default:
            break;
        }
    }
}