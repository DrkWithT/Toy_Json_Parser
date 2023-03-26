#ifndef JSON_PROPERTY_H
#define JSON_PROPERTY_H

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
        void *chunk;  // non-primitive (Array or Object)
    } data;
} Property;

Property *Property_Int(char *name, int value);
Property *Property_Float(char *name, float value);
Property *Property_String(char *name, char *value);
Property *Property_Chunk(char *name, void *value, DataType type);
void Property_Destroy(Property *self);
int Property_AsInt(const Property *self);
float Property_AsFloat(const Property *self);
const char *Property_AsStr(const Property *self);

/**
 * @brief Returns a void pointer and sets an external type code to determine whether the voidptr chunk is an array or object. 
 * 
 * @param self 
 * @param type_flag 
 * @return const void* 
 */
const void *Property_AsChunk(const Property *self, DataType *type_flag);

#endif