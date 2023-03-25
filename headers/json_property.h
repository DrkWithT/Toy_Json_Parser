#ifndef JSON_PROPERTY_H
#define JSON_PROPERTY_H

#include "json_array.h"

typedef struct json_property
{
    /* data */
    char *name;
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
} Property;

Property *Property_Int(char *name, int value);
Property *Property_Float(char *name, float value);
Property *Property_String(char *name, char *value);
Property *Property_Array(char *name, Array *value);
Property *Property_Object(char *name, Object *value);
void Property_Destroy(Property *self);
int Property_AsInt(const Property *self);
float Property_AsFloat(const Property *self);
const char *Property_AsStr(const Property *self);
const Array *Property_AsArr(const Property *self);
const Object *Property_AsObj(const Property *self);

#endif