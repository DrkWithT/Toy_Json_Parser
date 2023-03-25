#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include "json_property.h"

typedef struct json_obj
{
    /* data */
    size_t bucket_count;
    Property **buckets;
} Object;

/**
 * @brief Creates and initializes a hashtable-based key-value object with load factor 0.5.
 * 
 * @param slots Count of actual items (half the bucket count).
 * @return Object*
 */
Object *Object_Create(size_t slots);
void Object_Destroy(Object *self);
void Object_SetItem(Object *self, const char *key, Property *prop_val);
const Property *Object_GetItem(Object *self, const char *key);

#endif