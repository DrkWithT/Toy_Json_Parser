#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include "json_types.h"
#include "json_object.h"

typedef struct json_array_item
{
    /* data */
    DataType type;
    union
    {
        /* data */
        int i;
        float f;
        char *str;
        Array *arr;
        Object *obj;
    } data;

    ArrayItem *next;
} ArrayItem;

ArrayItem *ArrayItem_Int(int value);
ArrayItem *ArrayItem_Float(float value);

/**
 * @brief Initialize a JSON array slot for a string.
 * 
 * @param str The C-String to be moved to the internal "str" pointer.
 * @return ArrayItem*
 */
ArrayItem *ArrayItem_String(char *str);

/**
 * @brief Frees any dynamic memory within an ArrayItem (mostly C-Strings).
 * 
 * @param self 
 */
void ArrayItem_Destroy(ArrayItem *self);

typedef struct json_array
{
    /* data */
    size_t length;
    ArrayItem *head;
} Array;

Array *Array_Create();
void Array_Destroy(Array *self);
size_t Array_Length(const Array *self);
const ArrayItem *Array_Get(const Array *self, size_t pos);
void Array_Push(Array *self, ArrayItem *item);

#endif