#ifndef JSON_THING_H
#define JSON_THING_H

#include "json_data.h"

typedef struct json_thing
{
    /* data */
    char *file_name;
    Property *root; // Cannot be named or a primitive!
} JsonThing;

/**
 * @brief Creates and initializes a JSON representation data structure. If allocation fails entirely, NULL is returned. If root allocation fails, the root is NULL.
 * @note The file_name param must be a statically allocated C-String!
 * @param file_name The name of the JSON file.
 * @return JsonThing*
 */
JsonThing *JsonThing_Create(const char* file_name, DataType root_type, size_t opt_prop_count);

/**
 * @brief Recursively destroys properties starting from the root anonymous property (Array or Object). Fails if root property is primitive since that is invalid JSON. 
 * 
 * @param self 
 */
void JsonThing_Destroy(JsonThing *self);

#endif