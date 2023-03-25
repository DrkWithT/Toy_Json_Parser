#ifndef JSON_THING_H
#define JSON_THING_H

#include "json_data.h"

typedef struct json_thing
{
    /* data */
    char *file_name;
    Property *root;
} JsonThing;

/**
 * @brief Creates and initializes a JSON representation data structure.
 * @note The file_name param must be a statically allocated C-String!
 * @param file_name The name of the JSON file.
 * @return JsonThing*
 */
JsonThing *JsonThing_Create(const char* file_name);

// @note: I need to implement a recursive function to free up memory in nested properties... Ahhhhh!
void JsonThing_Destroy(JsonThing *self);

#endif