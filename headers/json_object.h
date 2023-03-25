#ifndef JSON_OBJ_H
#define JSON_OBJ_H

#include "json_data.h"

typedef struct json_object
{
    /* data */
    char *file_name;
    Property *root;
} JsonObject;

/**
 * @brief Creates and initializes a JSON representation data structure.
 * @note The file_name param must be a statically allocated C-String!
 * @param file_name The name of the JSON file.
 * @return JsonObject*
 */
JsonObject *JsonObject_Create(const char* file_name);

// @note: I need to implement a recursive function to free up memory in nested properties... Ahhhhh!
// void JsonObject_Destroy(JsonObject *self);

#endif