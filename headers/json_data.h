#ifndef JSON_LIST_H
#define JSON_LIST_H

/**
 * @file json_data.h
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Contains data structures to model JSON data: primitives, objects (hash tables TODO), and arrays (linked lists).
 * @note 1: All string initializations expect a dynamically allocated string to be move assigned by pointer.
 * @note 2: All ..._Destroy functions only free up internal data of a refernced struct.
 * @date 2023-03-24
 */

#include <stdlib.h>

/// Enums:

typedef enum json_type {
    INT,
    FLO,
    STR,
    ARR,
    OBJ
} DataType;

/// Objects:

typedef union json_value
{
    /* data */
    int i;
    float f;
    char *str;
    Array *arr;
} Value;

typedef struct json_array_item
{
    /* data */
    DataType type;
    Value data;
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

typedef struct json_property
{
    /* data */
    char *name;
    DataType type;
    Value data;
} Property;

Property *Property_Int(char *name, int value);
Property *Property_Float(char *name, float value);
Property *Property_String(char *name, char *value);
Property *Property_Array(char *name, Array *value);
void Property_Destroy(Property *self);
int Property_AsInt(const Property *self);
float Property_AsFloat(const Property *self);
const char *Property_AsStr(const Property *self);
const Array *Property_AsArr(const Property *self);

#endif